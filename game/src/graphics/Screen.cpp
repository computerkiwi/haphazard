#include <cstdarg>
#include <cstdlib>
#include <ctime>
#include "Screen.h"
#include "Shaders.h"
#include "Settings.h"
#include "Mesh.h"
#include "Transform.h"
#include "Texture.h"

#include "GL\glew.h"

#include <imgui.h>
#include "Imgui\imgui-setup.h"

///
// Screen
///

void Screen::Use() 
{ 
	mView.Use(); 
	glEnable(GL_DEPTH_TEST);
	mView.Clear(); 
}

void Screen::SetBackgroundColor(float r, float g, float b, float a)
{
	mView.SetClearColor(r, g, b, a);
	mFX.SetClearColor(1 - r, 1 - g, 1 - b, a); // Set FX as negative for debug
}

void Screen::SetEffects(int c, FX fx[])
{
	mFXList.clear();
	for (int i = 0; i < c; ++i)
	{
		mFXList.push_back(fx[i]);
	}
}

void Screen::AddEffect(FX fx)
{
	mFXList.push_back(fx);
}

void Screen::SetBlurAmount(float amt)
{
	blurAmount = amt;
	Shaders::ScreenShader::Blur->SetVariable("Intensity", amt);
	/* Use this when blurring via multi-pass gaus blur to save passes
	blurAmount = amt > 10 ? 10 : amt;
	if(amt > 10)
		Shaders::ScreenShader::Blur->SetVariable("Intensity", amt / 10.0f);
	else
		Shaders::ScreenShader::Blur->SetVariable("Intensity", 1.0f);
	*/
}

void Screen::RenderBlur(GLuint colorBuffer, FrameBuffer& target)
{
	static FrameBuffer pingpongFBO[2] = { FrameBuffer(1), FrameBuffer(1) };

	bool horizontal = true, first_iteration = true;

	int blurSmooth = 4;

	Shaders::ScreenShader::Blur->Use();
	mFullscreen.Bind();

	for (int i = 0; i < blurSmooth /*blurAmount*/; i++)
	{
		pingpongFBO[horizontal].Use();
		Shaders::ScreenShader::Blur->SetVariable("Horizontal", horizontal);
		if (first_iteration)
		{
			glBindTexture(GL_TEXTURE_2D, colorBuffer); // Take from source
			first_iteration = false;
		}
		else
			pingpongFBO[!horizontal].BindColorBuffer();

		mFullscreen.DrawTris();
		horizontal = !horizontal;
	}
	target.Use();
	mFullscreen.DrawTris(); // Draw final product onto target
}

void Screen::RenderBloom(FrameBuffer& source, FrameBuffer& target)
{
	static FrameBuffer blurredBrights; // Framebuffer to hold blurred brights in
	blurredBrights.Clear();

	// Draw extracted brights to target color buffers
	Shaders::ScreenShader::ExtractBrights->Use();
	target.Use();
	target.Clear();
	source.BindColorBuffer();
	mFullscreen.Bind();
	mFullscreen.DrawTris();

	// Target now contains (0) source screen, and (1) extracted brights (raw)
	RenderBlur(target.ColorBuffer(0), blurredBrights); // Draw blurred brights onto new framebuffer

	// Add blurred brights onto scene 
	Shaders::ScreenShader::Bloom->Use();
	target.Use();

	//Set textures in bloom shader to correct colorbuffers
	glActiveTexture(GL_TEXTURE0);
	source.BindColorBuffer();
	glActiveTexture(GL_TEXTURE1);
	blurredBrights.BindColorBuffer();

	mFullscreen.DrawTris();

	glActiveTexture(GL_TEXTURE0); // Reset
}

bool Screen::UseFxShader(FX fx, FrameBuffer& source, FrameBuffer& target)
{
	switch (fx)
	{
	case FX::BLUR:
		RenderBlur(source.ColorBuffer(), target);
		return false;
	case FX::BLUR_CORNERS:
		Shaders::ScreenShader::BlurCorners->Use();
		return true;
	case FX::EDGE_DETECTION:
		Shaders::ScreenShader::EdgeDetection->Use();
		return true;
	case FX::SHARPEN:
		Shaders::ScreenShader::Sharpen->Use();
		return true;
	case FX::BLOOM:
		RenderBloom(source, target);
		return false;
	default:
		Shaders::ScreenShader::Default->Use();
		return true;
	}

}

void Screen::Draw()
{
	FrameBuffer result = mView; // Resulting framebuffer after fx are applied (if applicable)

	if (mFXList.size() > 0)
	{
		FrameBuffer source = mView;
		FrameBuffer target = mFX;

		mFX.Clear(); // Who knows what is in mFX (probably the last frame or something, but clear it or problems will be.)
		glDisable(GL_DEPTH_TEST);

		mFullscreen.Bind(); // Bind screen mesh

		for (auto i = mFXList.begin(); i < mFXList.end(); ++i)
		{
			target.Use(); // Render to target

			if (UseFxShader(*i, source, target)) // Use next FX shader on list, returns if uses default rendering
			{
				//No special render method, render as normal texture
				source.BindColorBuffer(); // Bind source screen to target screen
				mFullscreen.DrawTris(); // Renders to screen other framebuffer
			}
			std::swap(source, target); // Render back and forth, applying another fx shader on each pass
		}

		result = source; // Save outcome colorbuffer (would be target, but they are swapped at the end of each loop)
	}
	
	Raindrop::DrawToScreen(result);

	// Enable Window framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Convert HDR to LDR
	Shaders::ScreenShader::HDR->Use(); 


	// Render final screen
	mFullscreen.Bind();
	glDisable(GL_DEPTH_TEST); // Dont want to lose screen to near clipping
	result.BindColorBuffer();
	mFullscreen.DrawTris();
	glEnable(GL_DEPTH_TEST);
}

void Screen::ResizeScreen(int width, int height)
{
	glViewport(0, 0, width, height);

	mView.SetDimensions(width, height);
	mFX.SetDimensions(width, height);
}

///
// FrameBuffer
///

Screen::FrameBuffer::FrameBuffer(int numColBfrs)
	: numColBfrs{numColBfrs}
{
	glGenFramebuffers(1, &mID);
	glBindFramebuffer(GL_FRAMEBUFFER, mID);
	glGenTextures(numColBfrs, mColorBuffers);
	SetDimensions(Settings::ScreenWidth(), Settings::ScreenHeight());
}

void Screen::FrameBuffer::SetDimensions(int width, int height)
{
	if (mWidth != width || mHeight != height)
	{
		mWidth = width;
		mHeight = height;
		GenerateColorBuffers();
		GenerateDepthStencilObject();
	}
}

void Screen::FrameBuffer::GenerateColorBuffers()
{
	Use();

	for (int i = 0; i < numColBfrs; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, mColorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mWidth, mHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Attach to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, mColorBuffers[i], 0);
	}

	// Tell OpenGL we are rendering to multiple colorbuffers
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
}

void Screen::FrameBuffer::GenerateDepthStencilObject()
{
	// create a renderbuffer object for depth and stencil attachment (won't be sampling these)
	glGenRenderbuffers(1, &mDepthStencilBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, mDepthStencilBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mWidth, mHeight); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthStencilBuffer); // now attach it
}

void Screen::FrameBuffer::Use()
{
	glBindFramebuffer(GL_FRAMEBUFFER, mID);
}

void Screen::FrameBuffer::BindColorBuffer(int attachment)
{
	glBindTexture(GL_TEXTURE_2D, mColorBuffers[attachment]);
}

void Screen::FrameBuffer::Clear()
{
	glBindFramebuffer(GL_FRAMEBUFFER, mID);
	glClearColor(mClearColor.x, mClearColor.y, mClearColor.z, mClearColor.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear buffers
}

void Screen::FrameBuffer::SetClearColor(float r, float g, float b, float a) 
{
	mClearColor = glm::vec4(r, g, b, a); 
}


///
// Mesh
///

Screen::Mesh::Mesh(float bottom, float top, float left, float right)
{
	bottom = bottom * 2 - 1;
	top = top * 2 - 1;
	left = left * 2 - 1;
	right = right * 2 - 1;

	float quadVerts[] =
	{
		left, bottom, 0, 0,
		right, bottom, 1, 0,
		right,  top, 1, 1,

		right,  top, 1, 1,
		left,  top, 0, 1,
		left, bottom, 0, 0,
	};

//	Shaders::ScreenShader::Default->Use();
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), &quadVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

Screen::Mesh::~Mesh()
{
	glDeleteBuffers(1, &mVAO);
	glDeleteVertexArrays(1, &mVBO);
}

void Screen::Mesh::Bind() 
{ 
	glBindVertexArray(mVAO); 
}

void Screen::Mesh::DrawTris() 
{ 
	glDrawArrays(GL_TRIANGLES, 0, 6); 
}


///
// Raindrop
///

Screen::FrameBuffer* Screen::Raindrop::drops;
Screen::FrameBuffer* Screen::Raindrop::screen;
std::list<Screen::Raindrop*> Screen::Raindrop::raindrops;

Texture* Screen::Raindrop::texture;
Screen::Mesh* Screen::Raindrop::mesh;

Screen::Raindrop::Raindrop()
{
	if (!texture)
		texture = new Texture("raindrop.png");
	if (!mesh)
		mesh = new Mesh();

	if (!drops)
	{
		drops = new FrameBuffer(1);
		srand(2);
	}
	if (!screen)
		screen = new FrameBuffer(1);

	transform.SetPosition(rand() % 200 / 100.0f - 1, rand() % 200 / 100.0f - 1, 0);
	float w = rand() % 100 / (100.0f / sizeMax.x) + sizeMin.x;
	float h = (w + rand() % 100 / (100.0f / (sizeMax.y - w))) * Settings::ScreenWidth() / Settings::ScreenHeight();
	transform.SetScale(w, -h, rand() % 100 / (100.0f / sizeMax.z) + sizeMin.z);

	life += rand() % 10 / 10 - 0.5f;
	speed = rand() % 100 / ((w / sizeMax.x) * 50.0f) + 0.1f;

	drops->SetClearColor(0, 0, 0, 0);
}

void Screen::Raindrop::DrawToScreen(Screen::FrameBuffer& dest)
{
	if (raindrops.size() == 0) // Don't need to render
		return;

	// Blur screen
	int b = GetView().blurAmount;
	GetView().SetBlurAmount(10);
	Screen::GetView().UseFxShader(BLUR, dest, *screen);
	GetView().SetBlurAmount(b);

	// Render raindrops onto fresh buffer
	drops->Use();
	drops->Clear();
	for(Raindrop* r : raindrops)
		r->Draw();
	
	// Set render to additive
	glBlendFunc(GL_SRC_ALPHA, GL_DST_COLOR);
	
	Shaders::ScreenShader::Default->Use();
	
	// Render drops to destination screen
	dest.Use();
	Screen::GetView().mFullscreen.Bind();
	drops->BindColorBuffer();
	Screen::GetView().mFullscreen.DrawTris();

	// Reset blend mode
	glBlendFunc(GL_ONE, GL_ZERO);
}

void Screen::Raindrop::Draw()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Set default blendmode

	Shaders::ScreenShader::Raindrop->Use();

	Shaders::ScreenShader::Raindrop->SetVariable("position", transform.GetPosition());
	Shaders::ScreenShader::Raindrop->SetVariable("scale", transform.GetScale());
	Shaders::ScreenShader::Raindrop->SetVariable("alpha", alpha);

	mesh->Bind();
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->GetID());
	glActiveTexture(GL_TEXTURE1);
	screen->BindColorBuffer();

	mesh->DrawTris();

	glActiveTexture(GL_TEXTURE0); // Reset active texture
}

float count = 0;

void Screen::UpdateRaindrops(float dt)
{
	count += dt;
	float f = 0.1f + rand() % 10 / 10.0f;
	if (count > f)
	{
		GetView().AddRaindrop();
		count=0;
	}


	for (Raindrop* r : Raindrop::raindrops)
	{
		r->Update(dt);
	}
	
	for (Raindrop* r : Raindrop::raindrops)
		if (r->life < 0)
		{
			Raindrop::raindrops.remove(r);
			delete r;
			break;
		}
}

void Screen::Raindrop::Update(float dt)
{
	transform.SetPosition(transform.GetPosition().x, transform.GetPosition().y - (dt * speed) / 40.0f, 0);
	transform.SetScale(transform.GetScale().x - transform.GetScale().x * (dt * speed) / 7.0f, transform.GetScale().y - transform.GetScale().y * dt / 7.0f, transform.GetScale().z - transform.GetScale().z * dt / 6.0f);

	life -= dt;

	if(alpha < 1 && life > 5)
		alpha += dt * 5;
	else if (life < 5)
		alpha = life / 5.0f;
	else
		alpha = 1;
}

void Screen::AddRaindrop()
{
	Raindrop::raindrops.push_back(new Raindrop());
}


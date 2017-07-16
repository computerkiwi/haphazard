#include <cstdarg>
#include "Screen.h"
#include "Shaders.h"
#include "Settings.h"


#include "GL\glew.h"

///
// Screen
///

void Graphics::Screen::Use() 
{ 
	mView.Use(); 
	glEnable(GL_DEPTH_TEST);
	mView.Clear(); 
}

void Graphics::Screen::SetBackgroundColor(float r, float g, float b, float a)
{
	mView.SetClearColor(r, g, b, a);
	mFX.SetClearColor(1 - r, 1 - g, 1 - b, a); // Set FX as negative for debug
}

void Graphics::Screen::SetDimensions(int w, int h) 
{
	glViewport(0, 0, w, h);
	mView.SetDimensions(w, h);
	mFX.SetDimensions(w, h);
}

void Graphics::Screen::SetEffects(int c, FX fx[])
{
	mFXList.clear();
	for (int i = 0; i < c; ++i)
	{
		mFXList.push_back(fx[i]);
	}
}

void Graphics::Screen::AddEffect(FX fx)
{
	mFXList.push_back(fx);
}

void Graphics::Screen::RenderBlur(GLuint colorBuffer, FrameBuffer& target)
{
	static FrameBuffer pingpongFBO[2] = { FrameBuffer(1), FrameBuffer(1) };

	bool horizontal = true, first_iteration = true;
	int amount = 10;

	Shaders::ScreenShader::Blur->Use();
	mFullscreen.Bind();

	for (int i = 0; i < amount; i++)
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

void Graphics::Screen::RenderBloom(FrameBuffer& source, FrameBuffer& target)
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

bool Graphics::Screen::UseFxShader(FX fx, FrameBuffer& source, FrameBuffer& target)
{
	switch (fx)
	{
	case Graphics::FX::BLUR:
		RenderBlur(source.ColorBuffer(), target);
		return false;
	case Graphics::FX::BLUR_CORNERS:
		Graphics::Shaders::ScreenShader::BlurCorners->Use();
		return true;
	case Graphics::FX::EDGE_DETECTION:
		Graphics::Shaders::ScreenShader::EdgeDetection->Use();
		return true;
	case Graphics::FX::SHARPEN:
		Graphics::Shaders::ScreenShader::Sharpen->Use();
		return true;
	case Graphics::FX::BLOOM:
		RenderBloom(source, target);
		return false;
	default:
		Graphics::Shaders::ScreenShader::Default->Use();
		return true;
	}

}

void Graphics::Screen::Draw()
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
	
	// Enable Window framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Convert HDR to LDR
	Shaders::ScreenShader::HDR->Use(); 

	// Render final screen
	mFullscreen.Bind();
	glDisable(GL_DEPTH_TEST); // Dont want to lose screen to near clipping
	result.BindColorBuffer();
	mFullscreen.DrawTris();
}



///
// FrameBuffer
///

Graphics::Screen::FrameBuffer::FrameBuffer(int numColBfrs)
	: numColBfrs{numColBfrs}
{
	glGenFramebuffers(1, &mID);
	glBindFramebuffer(GL_FRAMEBUFFER, mID);

	mWidth = Settings::ScreenWidth();
	mHeight = Settings::ScreenHeight();

	GenerateColorBuffers();
	GenerateDepthStencilObject();
}

void Graphics::Screen::FrameBuffer::SetDimensions(int width, int height)
{
	if (mWidth != width || mHeight != height)
	{
		mWidth = width;
		mHeight = height;
		GenerateColorBuffers();
		GenerateDepthStencilObject();
	}
}

void Graphics::Screen::FrameBuffer::GenerateColorBuffers()
{
	// create a color attachment texture
	glGenTextures(numColBfrs, mColorBuffers);

	for (int i = 0; i < numColBfrs; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, mColorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mWidth, mHeight, 0, GL_RGB, GL_FLOAT, NULL);
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

void Graphics::Screen::FrameBuffer::GenerateDepthStencilObject()
{
	// create a renderbuffer object for depth and stencil attachment (won't be sampling these)
	glGenRenderbuffers(1, &mDepthStencilBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, mDepthStencilBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mWidth, mHeight); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthStencilBuffer); // now attach it
}

void Graphics::Screen::FrameBuffer::Use()
{
	glBindFramebuffer(GL_FRAMEBUFFER, mID);
}

void Graphics::Screen::FrameBuffer::BindColorBuffer(int attachment)
{
	glBindTexture(GL_TEXTURE_2D, mColorBuffers[attachment]);
}

void Graphics::Screen::FrameBuffer::Clear()
{
	glBindFramebuffer(GL_FRAMEBUFFER, mID);
	glClearColor(mClearColor.x, mClearColor.y, mClearColor.z, mClearColor.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear buffers
}

void Graphics::Screen::FrameBuffer::SetClearColor(float r, float g, float b, float a) 
{
	mClearColor = glm::vec4(r, g, b, a); 
}


///
// Mesh
///

Graphics::Screen::Mesh::Mesh(float bottom, float top, float left, float right)
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

void Graphics::Screen::Mesh::Bind() 
{ 
	glBindVertexArray(mVAO); 
}

void Graphics::Screen::Mesh::DrawTris() 
{ 
	glDrawArrays(GL_TRIANGLES, 0, 6); 
}

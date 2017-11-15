#include "RenderLayer.h"
#include "graphics\Settings.h"
#include "Screen.h"
#include "Shaders.h"

FrameBuffer* FrameBuffer::fb_FX = nullptr;
static Screen::Mesh* fullscreenMesh = nullptr;

// Shader Framebuffers
static FrameBuffer* blur_pingpongFBO[2];
static FrameBuffer* bloom_blurredBrights;

///
// FrameBuffer
///

void FrameBuffer::InitFrameBuffers()
{
	fb_FX = new FrameBuffer(-999);
	fullscreenMesh = Screen::m_Fullscreen; //new Screen::Mesh();
	Screen::m_LayerList.clear(); // Take out the fx buffer framebuffer from the render list
	blur_pingpongFBO[0] = new FrameBuffer(-999);
	blur_pingpongFBO[1] = new FrameBuffer(-999);
	bloom_blurredBrights = new FrameBuffer(-999);
}

void FrameBuffer::ResizePrivateFrameBuffers(int w, int h)
{
	fb_FX->SetDimensions(w, h);
	blur_pingpongFBO[0]->SetDimensions(w, h);
	blur_pingpongFBO[1]->SetDimensions(w, h);
	bloom_blurredBrights->SetDimensions(w, h);
}

FrameBuffer::FrameBuffer(int layer, int numColBuffers)
	: m_NumColBfrs{ numColBuffers }, m_Layer{ layer }
{
	glGenFramebuffers(1, &m_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
	glGenTextures(m_NumColBfrs, m_ColorBuffers);

	m_Width = Settings::ScreenWidth();
	m_Height = Settings::ScreenHeight();

	glBindTexture(GL_TEXTURE_2D, m_ColorBuffers[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// Attach to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorBuffers[0], 0);

	Screen::m_LayerList.insert(this);
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &m_ID);
	glDeleteTextures(m_NumColBfrs, m_ColorBuffers);
	//glDeleteRenderbuffers(1, &m_DepthStencilBuffer);
}

void FrameBuffer::SetDimensions(int width, int height)
{
	if (m_Width != width || m_Height != height)
	{
		m_Width = width;
		m_Height = height;
		
		// Free old buffers
		//glDeleteTextures(m_NumColBfrs, m_ColorBuffers);
		//glDeleteRenderbuffers(1, &m_DepthStencilBuffer);

		// Generate new buffers
		GenerateColorBuffers();
		//GenerateDepthStencilObject();
	}
}

void FrameBuffer::GenerateColorBuffers()
{
	Use();

	for (int i = 0; i < m_NumColBfrs; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, m_ColorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, NULL);
	}
}

void FrameBuffer::GenerateDepthStencilObject()
{
	return;
	// create a renderbuffer object for depth and stencil attachment (won't be sampling these)
	glGenRenderbuffers(1, &m_DepthStencilBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_DepthStencilBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthStencilBuffer); // now attach it
}

void FrameBuffer::Use()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
	m_UsedThisUpdate = true;
}

void FrameBuffer::AddEffect(FX fx)
{
	m_FXList.push_back(fx);
}

void FrameBuffer::SetEffects(int c, FX fx[])
{
	m_FXList.clear();
	for (int i = 0; i < c; ++i)
	{
		m_FXList.push_back(fx[i]);
	}
}

void FrameBuffer::BindColorBuffer(int attachment)
{
	glBindTexture(GL_TEXTURE_2D, m_ColorBuffers[attachment]);
}

void FrameBuffer::Clear()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
	glClearColor(m_ClearColor.x, m_ClearColor.y, m_ClearColor.z, m_ClearColor.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear buffers
}

void FrameBuffer::SetClearColor(float r, float g, float b, float a)
{
	m_ClearColor = glm::vec4(r, g, b, a);
}

void FrameBuffer::RenderEffects()
{
	if (m_FXList.size() == 0) // No PPFX to apply. Return.
		return;

	FrameBuffer* source = this;
	FrameBuffer* target = fb_FX;

	fb_FX->Clear();

	fullscreenMesh->Bind(); // Bind screen mesh

	for (auto i = m_FXList.begin(); i < m_FXList.end(); ++i)
	{
		target->Use(); // Render to target

		ApplyFXSettings(*i); // Apply FX uniform values (if applicable)

		if (UseFxShader(*i, *source, *target)) // Use next FX shader on list, returns if uses default rendering
		{
			//No special render method, render as normal texture
			source->BindColorBuffer(); // Bind source screen to target screen
			fullscreenMesh->DrawTris(); // Renders to screen other framebuffer
		}
		std::swap(source, target); // Render back and forth, applying another fx shader on each pass
	}

	// If end result is not already this framebuffer, copy the framebuffer
	if (this != source)
	{
		/*glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
		fullscreenMesh->Bind();
		source->BindColorBuffer();
		fullscreenMesh->DrawTris();
		*/

		
		// Blit fx framebuffer to this framebuffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_FX->m_ID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ID);
		glBlitFramebuffer(0, 0, fb_FX->m_Width, fb_FX->m_Height, 0, 0, m_Width, m_Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}
}


///
// Post Processing Effect Variable Functions
/// 

void FrameBuffer::ApplyFXSettings(FX fx)
{
	switch (fx)
	{
	case FX::BLUR:
		Shaders::ScreenShader::Blur->SetVariable("Intensity", m_BlurAmount);
		return;
	case FX::BLUR_CORNERS:
		return;
	case FX::EDGE_DETECTION:
		return;
	case FX::SHARPEN:
		return;
	case FX::BLOOM:
		Shaders::ScreenShader::Blur->SetVariable("Intensity", m_BlurAmount);
		return;
	default:
		return;
	}
}

void FrameBuffer::SetBlurAmount(float amt)
{
	m_BlurAmount = amt;
}

///
// Post Processing Effect Rendering Functions
///


bool FrameBuffer::UseFxShader(FX fx, FrameBuffer& source, FrameBuffer& target)
{
	switch (fx)
	{
	case FX::BLUR:
		RenderBlur(source.GetColorBuffer(), target);
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


void FrameBuffer::RenderBlur(GLuint colorBuffer, FrameBuffer& target)
{
	bool horizontal = true, first_iteration = true;

	blur_pingpongFBO[0]->Clear();
	blur_pingpongFBO[1]->Clear();

	int blurSmooth = 4;

	Shaders::ScreenShader::Blur->Use();
	fullscreenMesh->Bind();

	for (int i = 0; i < blurSmooth /*blurAmount*/; i++)
	{
		blur_pingpongFBO[horizontal]->Use();
		Shaders::ScreenShader::Blur->SetVariable("Horizontal", horizontal);
		if (first_iteration)
		{
			glBindTexture(GL_TEXTURE_2D, colorBuffer); // Take from source
			first_iteration = false;
		}
		else
			blur_pingpongFBO[!horizontal]->BindColorBuffer();

		fullscreenMesh->DrawTris();
		horizontal = !horizontal;
	}
	//target.Use();
	//fullscreenMesh->DrawTris(); // Draw final product onto target

	glBindFramebuffer(GL_READ_FRAMEBUFFER, blur_pingpongFBO[!horizontal]->m_ID);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target.m_ID);
	glBlitFramebuffer(0, 0, target.m_Width, target.m_Height, 0, 0, target.m_Width, target.m_Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void FrameBuffer::RenderBloom(FrameBuffer& source, FrameBuffer& target)
{
	bloom_blurredBrights->Clear();

	// Draw extracted brights to target color buffers
	Shaders::ScreenShader::ExtractBrights->Use();
	target.Use();
	target.Clear();
	source.BindColorBuffer();
	fullscreenMesh->Bind();
	fullscreenMesh->DrawTris();

	// Target now contains (0) source screen, and (1) extracted brights (raw)
	RenderBlur(target.GetColorBuffer(0), *bloom_blurredBrights); // Draw blurred brights onto new framebuffer

													   // Add blurred brights onto scene 
	Shaders::ScreenShader::Bloom->Use();
	target.Use();

	//Set textures in bloom shader to correct colorbuffers
	glActiveTexture(GL_TEXTURE0);
	source.BindColorBuffer();
	glActiveTexture(GL_TEXTURE1);
	bloom_blurredBrights->BindColorBuffer();

	fullscreenMesh->DrawTris();

	glActiveTexture(GL_TEXTURE0); // Reset
}

#include <cstdarg>
#include "Graphics.h"

Graphics::Screen::Screen(FX screenEffect, int width, int height, float effectIntensity)
		: mWidth{ width }, mHeight{ height }, mIntensity{ effectIntensity }
{
	glGenFramebuffers(1, &mID);
	glBindFramebuffer(GL_FRAMEBUFFER, mID);

	switch (screenEffect)
	{
	default:
		mShader = Shaders::ScreenShader::Default;
		break;
	case EDGE_DETECTION:
		mShader = Shaders::ScreenShader::EdgeDetection;
		break;
	case SHARPEN:
		mShader = Shaders::ScreenShader::Sharpen;
		break;
	case BLUR:
		mShader = Shaders::ScreenShader::Blur;
		break;
	case BLUR_CORNERS:
		mShader = Shaders::ScreenShader::BlurCorners;
		break;
	case BLUR_FOCUS:
		mShader = Shaders::ScreenShader::BlurFocus;
		break;
	}

	GenerateColorBuffer();
	GenerateDepthStencilObject();
}

//Check if current screen is usable
bool Graphics::Screen::CurrentScreenIsComplete()
{
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;
	return true;
}

void Graphics::Screen::SetSize(int width, int height)
{
	mWidth = width;
	mHeight = height;
	GenerateColorBuffer();
	GenerateDepthStencilObject();
}

void Graphics::Screen::Use(bool clearScreen)
{
	glBindFramebuffer(GL_FRAMEBUFFER, mID);
	glEnable(GL_DEPTH_TEST);
	if(clearScreen)
		Clear();
}

void Graphics::Screen::Clear()
{
	glClearColor(mClearColor.x, mClearColor.y, mClearColor.z, mClearColor.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear buffers
}

void Graphics::Screen::ClearStencil()
{
	glClear(GL_STENCIL_BUFFER_BIT);
}

void Graphics::Screen::GenerateColorBuffer()
{
	// create a color attachment texture
	glGenTextures(1, &mColorBuffer);
	glBindTexture(GL_TEXTURE_2D, mColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorBuffer, 0);
}

void Graphics::Screen::GenerateDepthStencilObject()
{
	// create a renderbuffer object for depth and stencil attachment (won't be sampling these)
	glGenRenderbuffers(1, &mDepthStencilBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, mDepthStencilBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mWidth, mHeight); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthStencilBuffer); // now actually attach it
}

// Variables should range from 0-1
Graphics::ScreenMesh::ScreenMesh(float bottom, float top, float left, float right)
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

// Draws screen on mesh
void Graphics::ScreenMesh::Draw(Screen& screen)
{
	glDisable(GL_DEPTH_TEST);

	screen.GetShader()->Use();
	screen.GetShader()->SetVariable("Intensity", screen.Intensity());

	glBindVertexArray(mVAO);
	glBindTexture(GL_TEXTURE_2D, screen.ColorBuffer());
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Graphics::ScreenMesh::DrawScreenOnScreen(Graphics::Screen& source, Graphics::Screen& dest)
{
	glDisable(GL_DEPTH_TEST);

	dest.Use();

	source.GetShader()->Use();
	source.GetShader()->SetVariable("Intensity", source.Intensity());

	glBindVertexArray(mVAO);
	glBindTexture(GL_TEXTURE_2D, source.ColorBuffer());
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Graphics::ScreenMesh::CompoundScreens(int count, ...)
{
	va_list args;
	va_start(args, count);
	Screen source = va_arg(args, Screen);
	for (int i = 0; i < count - 1; ++i) {
		Screen dest = va_arg(args, Screen);

		DrawScreenOnScreen(source, dest);

		source = dest;
	}
	va_end(args);
}

Graphics::ScreenMesh::~ScreenMesh()
{
	glDeleteBuffers(1, &mVBO);
	glDeleteVertexArrays(1, &mVAO);
}

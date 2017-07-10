#include "Graphics.h"

Graphics::Screen::Screen(FX screenEffect, int width, int height, float effectIntensity)
		: width{ width }, height{ height }, intensity{ effectIntensity }
{
	glGenFramebuffers(1, &mID);
	glBindFramebuffer(GL_FRAMEBUFFER, mID);

	switch (screenEffect)
	{
	default:
		shader = Shaders::defaultScreenShader;
	case EDGE_DETECTION:

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

void Graphics::Screen::GenerateColorBuffer()
{
	// create a color attachment texture
	glGenTextures(1, &mColorBuffer);
	glBindTexture(GL_TEXTURE_2D, mColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorBuffer, 0);
}

void Graphics::Screen::GenerateDepthStencilObject()
{
	// create a renderbuffer object for depth and stencil attachment (won't be sampling these)
	glGenRenderbuffers(1, &mDepthStencilBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, mDepthStencilBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthStencilBuffer); // now actually attach it
}

// Variables should range from 0-1
Graphics::ScreenMesh::ScreenMesh(Screen& screen, float bottom, float top, float left, float right)
	: mScreen(screen)
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
void Graphics::ScreenMesh::Draw()
{
	glBindVertexArray(mVAO);
	glBindTexture(GL_TEXTURE_2D, mScreen.ColorBuffer());
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

Graphics::ScreenMesh::~ScreenMesh()
{
	glDeleteBuffers(1, &mVBO);
	glDeleteVertexArrays(1, &mVAO);
}

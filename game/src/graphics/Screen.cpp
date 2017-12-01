/*
FILE: Screen.cpp
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include <cstdarg>
#include <cstdlib>
#include <ctime>
#include "Screen.h"
#include "Shaders.h"
#include "Settings.h"
#include "Mesh.h"
#include "Texture.h"

#include "GL\glew.h"

#include <imgui.h>
#include "Imgui\imgui-setup.h"

///
// Screen
///

FrameBuffer* Screen::m_View;
Screen::Mesh* Screen::m_Fullscreen;
std::set<FrameBuffer*, LayerComp> Screen::m_LayerList;


void Screen::InitScreen()
{
	m_Fullscreen = new Mesh();

	FrameBuffer::InitFrameBuffers();
	m_View = new FrameBuffer(0);
}

void Screen::Use() 
{ 
	m_View->Clear(); 

	for (FrameBuffer* fb : m_LayerList)
	{
		fb->Clear();
		fb->m_UsedThisUpdate = false;
	}

	m_View->Use(); 
}

void Screen::SetBackgroundColor(float r, float g, float b, float a)
{
	m_View->SetClearColor(r, g, b, a);
}

void Screen::Draw()
{
	glDisable(GL_BLEND);
	for (FrameBuffer* fb : m_LayerList)
		if(fb->m_UsedThisUpdate)
			fb->RenderEffects();

	//Draw Layers to View framebuffer
	Shaders::ScreenShader::Default->Use();
	//glBindFramebuffer(GL_FRAMEBUFFER, m_View->m_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	m_Fullscreen->Bind();
	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (FrameBuffer* fb : m_LayerList)
	{ 
		//glBindFramebuffer(GL_READ_FRAMEBUFFER, fb->m_ID);
		//glBlitFramebuffer(0, 0, fb->m_Width, fb->m_Height, 0, 0, m_View->m_Width, m_View->m_Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		fb->BindColorBuffer();
		m_Fullscreen->DrawTris();
	}
	glDisable(GL_BLEND);
	
	//Raindrop::DrawToScreen(*result);

	// Enable Window framebuffer

	/*
	// Convert HDR to LDR
	Shaders::ScreenShader::HDR->Use(); 


	// Render final screen
	m_Fullscreen->Bind();
	m_View->BindColorBuffer();
	m_Fullscreen->DrawTris();

	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_View->m_ID);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, m_View->m_Width, m_View->m_Height, 0, 0, Settings::ScreenWidth(), Settings::ScreenHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
	*/

	for (auto fb = m_LayerList.begin(); fb != m_LayerList.end();)
	{
		auto test = fb++;
		if (!(*test)->m_UsedThisUpdate && (*test)->m_FXList.size() == 0)
		{
			delete *test;
			m_LayerList.erase(test); // Empty framebuffer, remove
		}
	}
	if (!(*m_LayerList.begin())->m_UsedThisUpdate)
		m_LayerList.erase(m_LayerList.begin());
}

void Screen::ResizeScreen(int width, int height)
{
	glViewport(0, 0, width, height);

	if (!m_View)
		return;

	m_View->SetDimensions(width, height);

	FrameBuffer::ResizePrivateFrameBuffers(width, height);
	
	for (FrameBuffer* fb : m_LayerList)
	{
		fb->SetDimensions(width, height);
	}
}

FrameBuffer* Screen::GetLayerFrameBuffer(int layer)
{
	for (FrameBuffer* fb : m_LayerList)
	{
		if (layer == fb->m_Layer)
			return fb;
	}
	
	return new FrameBuffer(layer);
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

/*
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
	int b = Screen::m_BlurAmount;
	Screen::SetBlurAmount(10);
	Screen::UseFxShader(BLUR, dest, *screen);
	Screen::SetBlurAmount(b);

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
	Screen::m_Fullscreen->Bind();
	drops->BindColorBuffer();
	Screen::m_Fullscreen->DrawTris();

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
		AddRaindrop();
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

*/
/*
FILE: Screen.h
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once
#include <glm/glm.hpp>
#include <set>

#include "Texture.h"
#include "RenderLayer.h"
#include "VertexObjects.h"
#include "TextureResource.h"

typedef unsigned int GLuint;

class Screen
{
	friend FrameBuffer; // Give access to add framebuffers to screen's list
	Screen() = delete; // Disable Default constructor
public:
	// Declaration
	class Mesh;

	///
	// Public Interface
	///
	static void InitScreen();

	static void Use(); // Start render to this screen (clears screen)
	static void Draw();

	static void RenderLoadingScreen();
	static void ResizeScreen(int width, int height);
	static void SetBackgroundColor(float r, float g, float b, float a);

	static FrameBuffer* GetLayerFrameBuffer(int layer);

	static std::set<FrameBuffer*, LayerComp>& GetLayerList() { return m_LayerList; }

private: // Variables
	static FrameBuffer* m_View;
	static Mesh* m_Fullscreen;
	static std::set<FrameBuffer*, LayerComp> m_LayerList;

public: // Helper Classes
	
	// Screen mesh (covers entire screen)
	class Mesh
	{
	public:
		Mesh(float bottom = 0, float top = 1, float left = 0, float right = 1);
		void Bind();
		void DrawTris();
		BufferObject m_VertexBuffer;
		VertexAttributeBindings m_AttribBindings;
	};

};

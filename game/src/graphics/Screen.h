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

	static void ResizeScreen(int width, int height);
	static void SetBackgroundColor(float r, float g, float b, float a);

	static FrameBuffer* GetLayerFrameBuffer(int layer);

	static std::set<FrameBuffer*, LayerComp>& GetLayerList() { return m_LayerList; }

	// Legacy (to be removed/changed soon)
	//static void AddRaindrop();
	//static void UpdateRaindrops(float dt);
	//

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
		~Mesh();
		void Bind();
		void DrawTris();
		GLuint mVAO, mVBO;
	};

/*
Legacy (to be removed soon)
	class Raindrop
	{
	public:
		Raindrop();
		static void DrawToScreen(Screen::FrameBuffer& screen);
		void Draw();
		void Update(float dt);

		static Screen::Mesh* mesh;
		static Texture* texture;
		Transform transform;
		float life = 7;
		float alpha = 0;
		float speed = 1;

		const glm::vec3 sizeMin = glm::vec3(0.05f, 0.03f, 0.4f);
		const glm::vec3 sizeMax = glm::vec3(0.1f, 0.15f, 0.7f);

		static FrameBuffer* drops;
		static FrameBuffer* screen;
		static std::list<Raindrop*> raindrops;
	};
*/
};

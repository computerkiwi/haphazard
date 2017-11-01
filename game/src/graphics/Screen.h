/*
FILE: Screen.h
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <list>

#include "Texture.h"

typedef unsigned int GLuint;

enum FX
{
	DEFAULT,
	EDGE_DETECTION,
	SHARPEN,
	BLUR,
	BLUR_CORNERS,
	BLOOM,
};

class Screen
{
	//Disable Defaults
	Screen() = delete;
public:
	// Declaration
	class FrameBuffer;
	class Mesh;

	///
	// Public Interface
	///
	static void InitScreen();

	static void Use(); // Start render to this screen (clears screen)
	static void Draw();

	static void ResizeScreen(int width, int height);
	static void SetBackgroundColor(float r, float g, float b, float a);

	static void SetEffects(int count, FX fx[]);
	static void AddEffect(FX fx);

	static void SetBlurAmount(float amt);


	// Legacy (to be removed/changed soon)
	//static void AddRaindrop();
	//static void UpdateRaindrops(float dt);
	//

private: // Private functions
	static bool UseFxShader(FX fx, FrameBuffer& source, FrameBuffer& target);

	static void RenderBloom(FrameBuffer& source, FrameBuffer& target);
	static void RenderBlur(GLuint colorBuffer, FrameBuffer& target);


private: // Variables
	static FrameBuffer* m_View;
	static FrameBuffer* m_FX;
	static Mesh* m_Fullscreen;
	static std::vector<FX> m_FXList;

	static float m_BlurAmount;


public: // Private classes
	class FrameBuffer
	{
		friend Screen;
	public:
		FrameBuffer(int numColBfrs = 2);
		void SetDimensions(int width, int height);
		void GenerateColorBuffers();
		void GenerateDepthStencilObject();
		void Use();
		void BindColorBuffer(int attachment = 0);
		void Clear();
		void SetClearColor(float r, float g, float b, float a);
		GLuint ColorBuffer(int attachment = 0) { return mColorBuffers[attachment]; }
	private:
		GLuint mID;
		GLuint mDepthStencilBuffer;
		GLuint mColorBuffers[2] = { 0,0 }; // Max color buffers = 2
		int mWidth, mHeight;
		int numColBfrs;
		glm::vec4 mClearColor;
	};

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

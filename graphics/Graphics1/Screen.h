#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <list>

#include "Texture.h"
#include "Transform.h"

typedef unsigned int GLuint;


namespace Graphics
{

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
		Screen(Screen const&) = delete;
		void operator=(Screen const&) = delete;

	public:
		static Screen& GetView()
		{
			static Screen s;
			return s;
		}

		void Use(); // Start render to this screen (clears screen)

		void SetBackgroundColor(float r, float g, float b, float a);
		void SetDimensions(int width, int height);
		void SetEffects(int count, FX fx[]);
		void AddEffect(FX fx);
		void SetBlurAmount(float amt);

		void AddRaindrop();
		static void UpdateRaindrops(float dt);

		void Draw();

		class FrameBuffer
		{
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
			GLuint mColorBuffers[2]; // Max color buffers = 2
			int mWidth, mHeight;
			int numColBfrs;
			glm::vec4 mClearColor;
		};

	private:

		class Mesh
		{
		public:
			Mesh(float bottom = 0, float top = 1, float left = 0, float right = 1);
			~Mesh();
			void Bind();
			void DrawTris();
			GLuint mVAO, mVBO;
		private:
		};

		class Raindrop
		{
		public:

			Raindrop();
			static void DrawToScreen(FrameBuffer& screen);
			void Draw();
			void Update(float dt);
			
			bool operator== (const Raindrop& r) const;

			static Screen::Mesh* mesh;
			static Texture* texture;
			Transform transform;
			float life = 7;
			float alpha = 0;
			float speed = 1;

			const glm::vec3 sizeMin = glm::vec3(0.05f, 0.03f, 0.4f);
			const glm::vec3 sizeMax = glm::vec3(0.1f,   0.15f, 0.7f);

			static FrameBuffer* drops;
			static FrameBuffer* screen;
			static std::list<Raindrop*> raindrops;
		};

		bool UseFxShader(FX fx, FrameBuffer& source, FrameBuffer& target);

		void RenderBloom(FrameBuffer& source, FrameBuffer& target);
		void RenderBlur(GLuint colorBuffer, FrameBuffer& target);

	private: // Variables
		FrameBuffer mView;
		FrameBuffer mFX;
		Mesh mFullscreen;
		std::vector<FX> mFXList;

		int blurAmount = 2;

		Screen()
		{
			mView = FrameBuffer();
			mFX = FrameBuffer();
			mFullscreen = Mesh();
		}
	};

};
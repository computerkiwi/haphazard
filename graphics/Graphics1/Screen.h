#pragma once

#include <glm/glm.hpp>
#include <vector>

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

		void Draw();

	private:
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

		class Mesh
		{
		public:
			Mesh(float bottom = 0, float top = 1, float left = 0, float right = 1);
			void Bind();
			void DrawTris();
		private:
			GLuint mVAO, mVBO;
		};

		bool UseFxShader(FX fx, FrameBuffer& source, FrameBuffer& target);

		void RenderBloom(FrameBuffer& source, FrameBuffer& target);
		void RenderBlur(GLuint colorBuffer, FrameBuffer& target);

	private: // Variables
		FrameBuffer mView;
		FrameBuffer mFX;
		Mesh mFullscreen;
		std::vector<FX> mFXList;

		Screen()
		{
			mView = FrameBuffer();
			mFX = FrameBuffer();
			mFullscreen = Mesh();
		}
	};

};
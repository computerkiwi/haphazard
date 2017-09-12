#pragma once

typedef unsigned int GLuint;

namespace Graphics
{

	class Texture
	{
	public:
		Texture(const char* file);
		Texture(float *pixels, int width, int height);
		Texture(GLuint id) { mID = id; }
		~Texture();

		GLuint GetID() const { return mID; }
	private:
		GLuint mID;
	};

};
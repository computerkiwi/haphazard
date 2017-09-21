#pragma once
#include "glm\glm.hpp"

typedef unsigned int GLuint;


class Texture
{
public:
	Texture(const char* file);
	Texture(float *pixels, int width, int height);
	Texture(GLuint id) { m_ID = id; }
	~Texture();

	GLuint GetID() const { return m_ID; }
private:
	GLuint m_ID;
};

class AnimatedTexture
{
public:
	AnimatedTexture(const char* file, int spriteWidth, int spriteHeight, int numSpritesX, int numSpritesY);

	// Returns top left of sprite frame
	glm::vec2 GetFrameCoords(int frame)
	{
		return glm::vec2(m_spriteWidth * (frame % m_spritesX), m_spriteHeight * (frame / m_spritesX));
	}

	glm::vec2 GetSpriteSize()
	{
		return glm::vec2(m_spriteWidth, m_spriteHeight);
	}

	int GetMaxFrame()
	{
		return m_spritesX * m_spritesY;
	}

	GLuint GetID() const { return m_ID; }

private:
	GLuint m_ID;
	float m_spriteWidth, m_spriteHeight;
	int m_spritesX, m_spritesY;
};

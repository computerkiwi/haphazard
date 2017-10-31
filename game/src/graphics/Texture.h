/*
FILE: Texture.h
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once
#include "glm\glm.hpp"

typedef unsigned int GLuint;
class AnimatedTexture;

class Texture
{
	friend class AnimatedTexture;
public:
	// Load texture from file
	Texture(const char* file);

	// Load texture from memory
	Texture(float *pixels, int width, int height);

	// Load texture from ID
	Texture(GLuint id) { m_ID = id; }

	~Texture();

	GLuint GetID() const { return m_ID; }
	glm::vec2 GetBounds();

	static void BindArray();
	
private:
	const static int MAX_WIDTH = 1920;
	const static int MAX_HEIGHT = 1080;

	static void GenerateTextureArray();

	static GLuint m_TextureArray;
	static GLuint m_layers;

	GLuint m_ID;
	float m_width, m_height;
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
	float m_width, m_height;
	float m_spriteWidth, m_spriteHeight;
	int m_spritesX, m_spritesY;
};

/*
FILE: Texture.h
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once
#include "Universal.h"
#include "Engine\ResourceManager.h"
#include "glm\glm.hpp"

typedef unsigned int GLuint;

class SpriteComponent;
class GameObject;
class Editor;

class Texture
{
public:
	// Load texture from file
	Texture(const char* file);

	virtual ~Texture();

	GLuint GetLayer() const { return m_layer; }
	virtual glm::vec4 GetBounds();

	static void BindArray();
	
	const static int MAX_WIDTH = 1920;
	const static int MAX_HEIGHT = 1080;

protected: // Static variables
	// Texture array
	static void GenerateTextureArray();
	static GLuint m_TextureArray;
	static GLuint m_layers;

private: // Variables
	GLuint m_layer;
	glm::vec4 m_bounds;
};

class AnimatedTexture : public Texture
{
public:
	AnimatedTexture(const char* file);
	AnimatedTexture(const char* file, int spriteWidth, int spriteHeight, int spritesX, int spritesY, int numSprites);
	
	// Returns top left of sprite frame
	virtual glm::vec4 GetBounds(int frame = 0);

	int GetMaxFrame() { return m_numSprites; }
	float GetDefaultFPS() {	return m_DefaultFPS; }

	void SetSpritesXY(int x, int y, int numSprites = 0);

private:
	float m_spriteWidth;
	float m_spriteHeight;

	int m_spritesX;
	int m_spritesY;
	int m_numSprites;

	float m_DefaultFPS;
};

class TextureHandler
{
	friend void ImGui_Sprite(SpriteComponent *sprite, GameObject object, Editor * editor);
public:
	TextureHandler(Resource* texture);
	void SetResource(Resource* texture);
	void Update(float dt);

	ResourceID GetResourceID() const { return m_Texture->Id(); }
	Texture* GetTexture() const { return reinterpret_cast<Texture*>(m_Texture->Data()); }

	void SetAnimatedTextureFPS(float fps);
	void SetAnimatedTextureFrame(float fps);

private:
	Resource* m_Texture;

	bool m_IsAnimated;
	int m_CurrentFrame = 0;
	float m_FPS = 0;
	float m_Timer = 0;
};
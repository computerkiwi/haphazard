/*
FILE: AnimationResource.h
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Texture.h"
#include "Engine/ResourceManager.h"

class AnimationResource : public Resource
{
private:
	// Inherited via Resource
	virtual void LoadData(const char * filePath) override;
	virtual void UnloadData() override;
	virtual void *GetData() override; // Returns a Texture *.
	virtual ResourceType GetType() override;

public:
	AnimationResource(const char *folderPath, const char *fileName);

private:
	AnimatedTexture* m_atex;
	std::string m_textureName;
	int m_spriteWidth;
	int m_spriteHeight;
	int m_spritesX;
	int m_spritesY;
	float m_FPS;
	int m_numSprites;
};


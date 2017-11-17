/*
FILE: AnimationResource.cpp
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "AnimationResource.h"

void AnimationResource::LoadData(const char *filePath)
{
	// Get this shit (member variables) from the file 
	m_atex = new AnimatedTexture(filePath, m_spriteWidth, m_spriteHeight, m_spritesX, m_spritesY, m_numSprites);
}

void AnimationResource::UnloadData()
{
	delete m_atex;
	m_atex = nullptr;
}

void *AnimationResource::GetData()
{
	return m_atex;
}

ResourceType AnimationResource::GetType()
{
	return ResourceType::ANIMATION;
}

AnimationResource::AnimationResource(const char *folderPath, const char *fileName)
	: Resource(folderPath, fileName), m_atex {nullptr}, m_FPS {0}, m_spriteWidth {1}, m_spriteHeight {1}, m_spritesX {1}, m_spritesY {1}, m_numSprites {1}
{
}


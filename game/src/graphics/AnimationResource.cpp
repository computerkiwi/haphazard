/*
FILE: AnimationResource.cpp
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "AnimationResource.h"
#include "Util/Serialization.h"

void AnimationResource::LoadData(const char *filePath)
{
	// Load the file.
	rapidjson::Document file = LoadJsonFile(filePath);
	auto temp = file.GetType();

	// Pull all the info we need out.
	m_textureName  = file["textureName"].GetString();
	m_spriteWidth  = file["frameWidth"].GetInt();
	m_spriteHeight = file["frameHeight"].GetInt();
	m_spritesX     = file["framesX"].GetInt();
	m_spritesY     = file["framesY"].GetInt();
	m_FPS          = file["fps"].GetFloat();
	m_numSprites   = file["frameCount"].GetInt();
	
	// Construct the texture's file path.
	std::string texturePath = "assets\\";
	texturePath.append(Resource::GetFolderName(ResourceType::TEXTURE));
	texturePath.append("\\");
	texturePath.append(m_textureName);

	// Create the texture.
	m_atex = new AnimatedTexture(texturePath.c_str(), m_spriteWidth, m_spriteHeight, m_spritesX, m_spritesY, m_numSprites, m_FPS);
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


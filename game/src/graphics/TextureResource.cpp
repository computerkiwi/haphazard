/*
FILE: TextureResource.cpp
PRIMARY AUTHOR: Kiera

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "TextureResource.h"

void TextureResource::LoadData(const char *filePath)
{
	m_tex = new Texture(filePath, m_isTiled);
}

void TextureResource::UnloadData()
{
	delete m_tex;
	m_tex = nullptr;
}

void *TextureResource::GetData()
{
	return m_tex;
}

ResourceType TextureResource::GetType()
{
	return ResourceType::TEXTURE;
}

TextureResource::TextureResource(const char *folderPath, const char *fileName) : Resource(folderPath, fileName), m_tex(nullptr)
{
	m_isTiled = std::string(fileName).find("Tile_") != std::string::npos; // Can it find Tile_ in the name?
}

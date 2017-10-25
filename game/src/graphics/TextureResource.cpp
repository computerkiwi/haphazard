#include "TextureResource.h"
/*
FILE: TextureResource.cpp
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/

void TextureResource::LoadData(const char *filePath)
{
	m_tex = new Texture(filePath);
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
}

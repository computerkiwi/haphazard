/*
FILE: TextureResource.h
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Texture.h"
#include "Engine/ResourceManager.h"

#define INVALID_TEXTURE_ID 0

class TextureResource : public Resource
{
private:
	// Inherited via Resource
	virtual void LoadData(const char * filePath) override;
	virtual void UnloadData() override;
	virtual void *GetData() override; // Returns a Texture *.
	virtual ResourceType GetType() override;

public:
	TextureResource(const char *folderPath, const char *fileName);

private:
	Texture *m_tex;
};

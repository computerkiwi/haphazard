/*
FILE: AudioResource.h
PRIMARY AUTHOR: Kiera

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Engine/ResourceManager.h"

#include "fmod.hpp"

class AudioResource : public Resource
{
public:
	// Inherited via Resource
	virtual void LoadData(const char * filePath) override;
	virtual void UnloadData() override;
	virtual void * GetData() override; // Returns an FMOD::Sound *
	virtual ResourceType GetType() override;

	AudioResource(const char * folderPath, const char * fileName);

private:
	FMOD::Sound *m_sound;
};

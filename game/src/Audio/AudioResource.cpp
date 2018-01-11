/*
FILE: AudioResource.cpp
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/

#include "Universal.h"

#include <cassert>

#include "AudioResource.h"
#include "AudioEngine.h"

#include "fmod_errors.h"

static void CheckErrorFMODInternal(FMOD_RESULT value, const char *function_name = "")
{
	if (value != FMOD_OK)
	{
		Logging::Log(Logging::AUDIO, Logging::HIGH_PRIORITY, "FMOD function ", function_name, " returned error: ", FMOD_ErrorString(value));
	}
}
#define CheckErrorFMOD(funcCall) CheckErrorFMODInternal(funcCall, #funcCall)

void AudioResource::LoadData(const char * filePath)
{
	FMOD::System *fmodSystem = Audio::GetSystem();

	Assert(fmodSystem != nullptr && "FMOD System is nullptr. Did you properly call Audio::Init() ?");

	CheckErrorFMOD(fmodSystem->createSound(filePath, FMOD_DEFAULT, nullptr, &m_sound));
}

void AudioResource::UnloadData()
{
	// TODO: Properly unload sounds.
}

void *AudioResource::GetData()
{
	return m_sound;
}

ResourceType AudioResource::GetType()
{
	return ResourceType::SOUND;
}

AudioResource::AudioResource(const char * folderPath, const char * fileName) : Resource(folderPath, fileName)
{
}

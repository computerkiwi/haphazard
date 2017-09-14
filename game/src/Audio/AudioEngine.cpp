/*
FILE: AudioEngine.cpp
PRIMARY AUTHOR: Kieran

Copyright © 2017 DigiPen (USA) Corporation.
*/

#include "Universal.h"

#include <string>
#include <assert.h>
#include <unordered_map>

#include "fmod.hpp"
#include "fmod_errors.h"

namespace Audio
{
	static FMOD::System *fmodSystem = nullptr;
	static std::unordered_map<std::string, FMOD::Sound *> loadedSounds;

	static void CheckErrorFMODInternal(FMOD_RESULT value, const char *function_name = "")
	{
		if (value != FMOD_OK)
		{
			Logging::Log(Logging::AUDIO, Logging::HIGH_PRIORITY, "FMOD function ", function_name, " returned error: ", FMOD_ErrorString(value));
		}
	}
	#define CheckErrorFMOD(funcCall) CheckErrorFMODInternal(funcCall, #funcCall)

	// Returns the path into the audio folder plus the filename.
	static std::string AudioAssetPath(const char *filename)
	{
		std::string filePath("assets\\audio\\");
		filePath.append(filename);
		return filePath;
	}

	void Init()
	{
		CheckErrorFMOD(FMOD::System_Create(&fmodSystem));

		const int MAX_CHANNELS = 32;
		CheckErrorFMOD(fmodSystem->init(MAX_CHANNELS, FMOD_INIT_NORMAL, nullptr));
	}

	void Update()
	{
		assert(fmodSystem != nullptr && "FMOD System is nullptr. Did you properly call Audio::Init() ?");

		fmodSystem->update();
	}

	void LoadSound(const char *fileName)
	{
		assert(fmodSystem != nullptr && "FMOD System is nullptr. Did you properly call Audio::Init() ?");

		std::string path = AudioAssetPath(fileName);

		FMOD::Sound **soundSlot = &loadedSounds[path];
		if ( *soundSlot == nullptr)
		{
			CheckErrorFMOD(fmodSystem->createSound(path.c_str(), FMOD_DEFAULT, nullptr, soundSlot));
		}
	}

	// Plays a given sound once.
	void PlaySound(const char *fileName, float volume)
	{
		assert(fmodSystem != nullptr && "FMOD System is nullptr. Did you properly call Audio::Init() ?");

		std::string path = AudioAssetPath(fileName);
		FMOD::Sound *sound = loadedSounds[path];

		if (sound == nullptr)
		{
			Logging::Log(Logging::AUDIO, Logging::HIGH_PRIORITY, "Attempted to play not loaded file", path);
			return;
		}

		FMOD::Channel *channel;
		fmodSystem->playSound(sound, nullptr, false, &channel);

		channel->setMode(FMOD_LOOP_OFF);
		channel->setVolume(volume);
	}
}

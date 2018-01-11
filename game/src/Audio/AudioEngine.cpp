/*
FILE: AudioEngine.cpp
PRIMARY AUTHOR: Kieran

Copyright � 2017 DigiPen (USA) Corporation.
*/

#include "Universal.h"

#include <string>
#include <assert.h>
#include <unordered_map>

#include "fmod.hpp"
#include "fmod_errors.h"
#include "AudioEngine.h"

#include "Engine/ResourceManager.h"

namespace Audio
{
	static FMOD::System *fmodSystem = nullptr;

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
		Assert(fmodSystem != nullptr && "FMOD System is nullptr. Did you properly call Audio::Init() ?");

		fmodSystem->update();
	}

	// Plays a given sound once.
	SoundHandle PlaySound(const char *fileName, float volume, float pitch, bool looping)
	{
		Assert(fmodSystem != nullptr && "FMOD System is nullptr. Did you properly call Audio::Init() ?");

		std::string file = fileName;

		if (ResourceManager::GetManager().Get(fileName) == nullptr)
		{
			Logging::Log(Logging::AUDIO, Logging::HIGH_PRIORITY, "Attempted to play unloaded audio file ", fileName);
			file = "default.wav";
		}

		Resource *resource = ResourceManager::GetManager().Get(file.c_str());
		FMOD::Sound *sound = reinterpret_cast<FMOD::Sound *>(resource->Data());

		FMOD::Channel *channel;
		fmodSystem->playSound(sound, nullptr, false, &channel);

		if (looping)
		{
			channel->setMode(FMOD_LOOP_NORMAL);
		}
		else
		{
			channel->setMode(FMOD_LOOP_OFF);
		}
		channel->setVolume(volume); // TODO: Fix volume not taking effect on music until another sound plays.
		channel->setPitch(pitch);

		return(SoundHandle(channel, resource->FileName().c_str()));
	}

	FMOD::System *GetSystem()
	{
		return fmodSystem;
	}

	SoundHandle::SoundHandle(FMOD::Channel *fmodChannel, const char *fileName) : m_fmodChannel(fmodChannel), m_fileName(fileName)
	{
	}

	bool SoundHandle::IsPlaying() const
	{
		bool isPlaying;
		CheckErrorFMOD(m_fmodChannel->isPlaying(&isPlaying));

		return isPlaying;
	}

	bool SoundHandle::IsLooping() const
	{
		FMOD_MODE mode;
		CheckErrorFMOD(m_fmodChannel->getMode(&mode));

		return (mode == FMOD_LOOP_NORMAL);
	}

	void SoundHandle::Stop()
	{
		CheckErrorFMOD(m_fmodChannel->stop());
	}

	void SoundHandle::SetPitch(float pitch)
	{
		CheckErrorFMOD(m_fmodChannel->setPitch(pitch));
	}

	float SoundHandle::GetPitch()
	{
		float pitch;
		CheckErrorFMOD(m_fmodChannel->getPitch(&pitch));

		return pitch;
	}
}

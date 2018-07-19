/*
FILE: AudioEngine.h
PRIMARY AUTHOR: Kieran

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Universal.h"

namespace FMOD
{
	class Channel;
	class System;
}

namespace Audio
{
	void Init();

	void Update();

	// Represents a currently playing sound.
	class SoundHandle
	{
	public:

		SoundHandle(FMOD::Channel *fmodChannel, const char *fileName);

		bool IsPlaying() const;

		bool IsLooping() const;

		void Stop();

		void SetPitch(float pitch);

		float GetPitch();

		void SetVolume(float volume);

		SoundHandle() : m_fmodChannel(nullptr) {}; // For meta.

		const char *GetFileName()
		{
			return m_fileName.c_str();
		}
	private:

		FMOD::Channel *m_fmodChannel;

		std::string m_fileName;

		META_NAMESPACE(Audio);
		META_REGISTER(SoundHandle)
		{
			META_DefineFunction(SoundHandle, IsPlaying, "IsPlaying");
			META_DefineFunction(SoundHandle, Stop, "Stop");

			META_DefineFunction(SoundHandle, GetFileName, "GetFileName");
		}
	};

	// Plays a given sound effect.
	SoundHandle PlaySound(const char *fileName, float volume = 1.0f, float pitch = 1.0f, bool looping = false);
	SoundHandle PlayMusic(const char *fileName, float volume = 1.0f, float pitch = 1.0f, bool loop = true);

	void ToggleSFX();
	void ToggleMusic();
	void SetQuietMusic(bool quiet);
	void SetMuted(bool muted);
	SoundHandle GetMusic();

	FMOD::System *GetSystem();
}

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

		SoundHandle(FMOD::Channel *fmodChannel);

		bool IsPlaying() const;

		bool IsLooping() const;

		void Stop();

		void SetPitch(float pitch);

		float GetPitch();

		SoundHandle() {}; // For meta.
	private:

		FMOD::Channel *m_fmodChannel;

		META_NAMESPACE(Audio);
		META_REGISTER(SoundHandle)
		{
			META_DefineFunction(SoundHandle, IsPlaying, "IsPlaying");
			META_DefineFunction(SoundHandle, Stop, "Stop");
		}
	};

	// Plays a given sound once.
	SoundHandle PlaySound(const char *fileName, float volume = 1.0f, float pitch = 1.0f, bool looping = false);

	FMOD::System *GetSystem();
}

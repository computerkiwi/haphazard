/*
FILE: AudioEngine.h
PRIMARY AUTHOR: Kieran

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

namespace FMOD
{
	class Channel;
}

namespace Audio
{
	void Init();

	void Update();

	void LoadSound(const char *fileName);


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

	private:
		FMOD::Channel *m_fmodChannel;
	};

	// Plays a given sound once.
	SoundHandle PlaySound(const char *fileName, float volume = 1.0f, float pitch = 1.0f, bool looping = false);
}

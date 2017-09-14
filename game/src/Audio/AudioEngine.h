/*
FILE: AudioEngine.h
PRIMARY AUTHOR: Kieran

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

namespace Audio
{
	void Init();

	void Update();

	void LoadSound(const char *fileName);

	// Plays a given sound once.
	void PlaySound(const char *fileName, float volume = 1.0f);
}

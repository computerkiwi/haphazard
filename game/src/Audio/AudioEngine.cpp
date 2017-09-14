/*
FILE: AudioEngine.cpp
PRIMARY AUTHOR: Kieran

Copyright © 2017 DigiPen (USA) Corporation.
*/

#include <string>

#include "fmod.hpp"

namespace HaphazardAudioTEMP_Namespace
{
	FMOD::System *system;
	FMOD::Sound  *sound;

	void Init(const char *soundFileName)
	{
		FMOD::System_Create(&system);

		const int MAX_CHANNELS = 32;
		system->init(MAX_CHANNELS, FMOD_INIT_NORMAL, nullptr);

		std::string path("assets\\audio\\fmod_examples\\");
		path += soundFileName;

		system->createSound(path.c_str(), FMOD_DEFAULT, nullptr, &sound);

		system->playSound(sound, nullptr, false, nullptr);
	}

	void Update()
	{
		system->update();
	}

}


void AudioTests()
{
	using namespace HaphazardAudioTEMP_Namespace;

	Init("swish.wav");

	while (true)
	{
		Update();
	}
}

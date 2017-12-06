/*
FILE: FrameCap.cpp
PRIMARY AUTHOR: Brett Schiff

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "FrameCap.h"
#include <Windows.h>

Timer::Timer() : m_startTime(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
{
}

// time passed so far in microseconds
__int64 Timer::timePassed()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - m_startTime;
}

// wait UNTIL a cetain amount of time has passed in microseconds
void Timer::waitUntil(__int64 time)
{
	__int64 currTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	__int64 sleepTime = (time - (currTime - m_startTime)) / 1000;
	if (sleepTime > 0)
	{
		Sleep((DWORD)sleepTime);
	}
}
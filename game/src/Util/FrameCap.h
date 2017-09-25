#pragma once

#include <chrono>

// timer in milliseconds
class Timer
{
public:
	// Constructor: starts timer
	Timer();

	// time passed so far in microseconds
	__int64 timePassed();

	// wait UNTIL a cetain amount of time has passed in microseconds
	void waitUntil(__int64 time);
	
private:
	__int64 m_startTime;
};
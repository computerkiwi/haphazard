/*
FILE: Logging.h
PRIMARY AUTHOR: Kieran Williams, Sweet

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>

class Logging
{
public:
	enum Priority
	{
		TRIVIAL_PRIORITY,
		LOW_PRIORITY,
		MEDIUM_PRIORITY,
		HIGH_PRIORITY,
		CRITICAL_PRIORITY
	};

	#define HAPHAZARD_LOGGING_CHANNEL_ENUM\
	LOGGING_CHANNEL(DEFAULT)\
	LOGGING_CHANNEL(LOGGING)\
	LOGGING_CHANNEL(PHYSICS)\
	LOGGING_CHANNEL(CORE)\
	LOGGING_CHANNEL(AUDIO)\
	LOGGING_CHANNEL(GRAPHICS)\
	LOGGING_CHANNEL(META)\
	LOGGING_CHANNEL(INPUT)\
	LOGGING_CHANNEL(SCRIPTING)\

	enum Channel
	{
		#define LOGGING_CHANNEL(CATEGORY) CATEGORY,
		HAPHAZARD_LOGGING_CHANNEL_ENUM
		#undef LOGGING_CHANNEL
	};

	static void Init(Engine *engine);
	static void Exit();

	static void Log(const char *message, Logging::Channel channel = Channel::DEFAULT, Priority priority = MEDIUM_PRIORITY);

	template <typename... Args>
	static void Log(Logging::Channel channel, Priority priority, Args&&... args)
	{
		std::stringstream str;
		ConstructVariadicLogString(str, std::forward<Args>(args)...);
		Log(str.str().c_str(), channel, priority);
	}

private:
	static bool m_logToFile;
	static bool m_logToConsole;

	static bool m_log;

	static std::thread m_loggingthread;
	static std::mutex m_mutex;

	static std::string m_writeBufferConsole;
	static std::string m_readBufferConsole;

	static std::string m_writeBufferFile;
	static std::string m_readBufferFile;

	static Priority m_consolePriority;

	static std::ofstream m_logFile;
	static Priority m_filePriority;


	static void LogToConsole(const char *message, Channel channel, Priority priority);

	static void LogToFile(const char *message, Channel channel, Priority priority);

	static bool TryOpenLogFile();

	static int vprintf(Logging::Channel channel, Priority priority, const char *format, ...);

	template <typename T, typename... Args>
	static void ConstructVariadicLogString(std::stringstream& str, const T& value, Args&&... args)
	{
		str << value;
		ConstructVariadicLogString(str, args...);
	}

	static void ConstructVariadicLogString(std::stringstream& str)
	{
		return;
	}
};

/*
FILE: Logging.h
PRIMARY AUTHOR: Kieran Williams, Sweet

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Util.h"
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>

class Engine;
class Logging;
class LoggingProxy;
//typedef char va_list;

class LoggingProxy
{
	Logging& m_logger;
public:
	LoggingProxy(Logging& logger_) : m_logger(logger_) {}

	template <typename T>
	LoggingProxy& operator <<(T& rhs)
	{
		std::stringstream ss;
		ss << rhs;
		m_logger.ObjectLog_ProxyAppend(ss.str().c_str());

		return *this;
	}
};


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

	static void Init();
	static void Exit();

	static void Log(const char *message, Channel channel = Channel::DEFAULT, Priority priority = MEDIUM_PRIORITY);
	static void Log_StartUp(const char *message, Channel channel, Priority priority);

	void ObjectLog(const char * message, Channel channel = Channel::DEFAULT, Priority priority = MEDIUM_PRIORITY);
	void ObjectLog_ProxyAppend(const char * message, Channel channel = Channel::DEFAULT, Priority priority = MEDIUM_PRIORITY);

	template <typename... Args>
	static void Log(Logging::Channel channel, Priority priority, Args&&... args)
	{
		std::stringstream str;
		ConstructVariadicLogString(str, std::forward<Args>(args)...);
		Log(str.str().c_str(), channel, priority);
	}

	template <typename... Args>
	static void Log_Editor(Logging::Channel channel, Priority priority, Args&&... args)
	{
		std::stringstream str;
		ConstructVariadicLogString(str, std::forward<Args>(args)...);
		Log_StartUp(str.str().c_str(), channel, priority);
	}

	template <typename T>
	LoggingProxy operator <<(T& rhs)
	{
		std::stringstream ss;
		ss << rhs;
		ObjectLog(ss.str().c_str());

		return LoggingProxy(*this);
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

	static int printf(Logging::Channel channel, Priority priority, const char *format, ...);
	static int vprintf(Logging::Channel channel, Priority priority, const char *format, va_list vars_copy);

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


extern Logging logger;


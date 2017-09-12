/*
FILE: Logging.h
PRIMARY AUTHOR: Kieran Williams, Sweet

Copyright © 2017 DigiPen (USA) Corporation.
*/
#include "Logging.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <locale>
#include <codecvt>

// Windows filesystem stuff.
#include <Windows.h>
#include <ShlObj.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

// Anonymous namespace for helper functions.
namespace
{
	// Creates a directory if it doesn't already exist.
	void TryCreateDirectory(const std::string& path)
	{
		// TODO[Kieran] - Error checking on Windows calls.

		if (PathFileExists(path.c_str()))
		{
			return;
		}

		CreateDirectory(path.c_str(), nullptr);
	}

	// Gets the path to the DigiPen/Haphazard folder in documents.
	std::string GetHaphazardFolderPath()
	{
		// TODO[Kieran] - Error checking on Windows calls.

		// Get the documents folder as a wide string.
		PWSTR pathPointer;
		SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DEFAULT, nullptr, &pathPointer);

		// Convert the wide string to a string.
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		std::string filePath = converter.to_bytes(pathPointer);

		// Setup the internal folders.
		filePath.append("\\DigiPen");
		TryCreateDirectory(filePath);

		filePath.append("\\Haphazard");
		TryCreateDirectory(filePath);

		return filePath;
	}

	// Takes a std::put_time format specifier.
	std::string GetTimeString(const char *formatSpecifier = "%c")
	{
		// Get the time.
		std::time_t currentTime = std::time(nullptr);
		std::tm localTime;
		localtime_s(&localTime, &currentTime);

		// Construct a filename.
		std::ostringstream timeString;
		timeString << std::put_time(&localTime, formatSpecifier);

		return timeString.str();
	}

	const char *GetLoggingChannelString(Logging::Channel category)
	{
		#define LOGGING_CHANNEL(CATEGORY) #CATEGORY ,
		const char* categories[] = { HAPHAZARD_LOGGING_CHANNEL_ENUM };
		#undef LOGGING_CHANNEL

		return categories[category];
	}

}


bool Logging::m_logToFile = true;
bool Logging::m_logToConsole = true;
bool Logging::m_log = true;

Logging::Priority Logging::m_consolePriority = Logging::MEDIUM_PRIORITY;

std::ofstream Logging::m_logFile = std::ofstream();
Logging::Priority Logging::m_filePriority = Logging::TRIVIAL_PRIORITY;

std::thread Logging::m_loggingthread;
std::mutex  Logging::m_mutex;

std::string Logging::m_writeBufferConsole;
std::string Logging::m_readBufferConsole;

std::string Logging::m_writeBufferFile;
std::string Logging::m_readBufferFile;


void Logging::Init()
{


	m_loggingthread = std::thread([]()
	{
		std::cout << "Side Thread: " << std::this_thread::get_id() << "\n";
		
		while (m_log)
		{
			// Check if we need to write
			if (m_writeBufferConsole.size())
			{
				// Lock the data and copy it out of the write buffer
				m_mutex.lock();
				m_readBufferConsole = m_writeBufferConsole;
				m_writeBufferConsole.clear();

				// Unlock the mutex
				m_mutex.unlock();

				std::cout << m_readBufferConsole;
				m_readBufferConsole.clear();
			}

			if (m_writeBufferFile.size())
			{
				// Set up the file if we haven't yet.
				TryOpenLogFile();

				// Lock the mutex
				m_mutex.lock();
				m_readBufferFile = m_writeBufferFile;
				m_writeBufferFile.clear();

				// Mutex unlock
				m_mutex.unlock();

				m_logFile << m_readBufferFile;
				m_readBufferFile.clear();
				
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(8));
		}
	});
}


void Logging::Exit()
{
	m_log = false;
	m_loggingthread.join();
}


void Logging::Log(const char *message, Logging::Channel channel, Priority priority)
{
	if (m_logToFile)
	{
		LogToFile(message, channel, priority);
	}
	if (m_logToConsole)
	{
		LogToConsole(message, channel, priority);
	}
}


void Logging::LogToConsole(const char * message, Logging::Channel channel, Priority priority)
{
	if (priority >= m_consolePriority)
	{
		std::stringstream ss;
		ss << GetTimeString("%X") << " [" << GetLoggingChannelString(channel) << "] " << message << std::endl;

		m_mutex.lock();
		m_writeBufferConsole += ss.str();
		m_mutex.unlock();
	}
}

void Logging::LogToFile(const char * message, Logging::Channel channel, Priority priority)
{
	if (priority >= m_filePriority)
	{
		std::stringstream ss;
		ss << GetTimeString("%D,%T") << ',' << GetLoggingChannelString(channel) << ',' << message << ',' << priority << std::endl;

		m_mutex.lock();
		m_writeBufferFile += ss.str();
		m_mutex.unlock();
	}
}

// Returns true if the file is open.
bool Logging::TryOpenLogFile()
{
	// If the file is already open, everything's fine.
	if (m_logFile.is_open())
	{
		return true;
	}

	// Otherwise, let's set up this file.

	// Get the time.
	std::time_t currentTime = std::time(nullptr);
	std::tm localTime;
	localtime_s(&localTime, &currentTime);

	// Construct a filename.
	std::ostringstream filename;
	filename << GetHaphazardFolderPath() << "\\haphazard_log_" << GetTimeString("%y_%m_%d_%H_%M_%S") << ".csv";

	// Open the file.
	m_logFile.open(filename.str().c_str());

	// If we couldn't open the file, log it (to the console I guess) and disable file logging.
	if (!m_logFile.is_open())
	{
		m_logToFile = false;

		Log(Channel::LOGGING, Logging::HIGH_PRIORITY, "Couldn't open log file \"", filename.str(), '"');

		return false;
	}

	// If we got here, the file is properly open.
	m_logFile << "DATE,TIME,CHANNEL,MESSAGE,PRIORITY" << std::endl;
	Log(Channel::LOGGING, Logging::MEDIUM_PRIORITY, "Succesfully opened log file ", filename.str());
	return true;
}

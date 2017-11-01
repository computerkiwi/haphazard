#include "ScriptResource.h"
/*
FILE: ScriptResource.cpp
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/

#include "Universal.h"

#include <fstream>

void ScriptResource::LoadData(const char *filePath)
{
	std::ifstream inStream;
	inStream.open(filePath);
	if (!inStream.is_open())
	{
		Logging::Log(Logging::Channel::SCRIPTING, Logging::HIGH_PRIORITY, "Couldn't open file ", filePath, " while loading a script.");
	}

	std::stringstream luaString;
	luaString << inStream.rdbuf();

	m_fileString = luaString.str();
}

void ScriptResource::UnloadData()
{
	// Don't even bother unloading the data. It's just a string.
}

void *ScriptResource::GetData()
{
	return &m_fileString;
}

ResourceType ScriptResource::GetType()
{
	return ResourceType::SCRIPT;
}

ScriptResource::ScriptResource(const char * folderPath, const char * fileName) : Resource(folderPath, fileName)
{
}

/*
FILE: ScriptResource.cpp
PRIMARY AUTHOR: Kiera

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "ScriptResource.h"
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

	// Make sure the lua file actually compiles.
	lua_State *L = GetGlobalLuaState();
	int currentStack = lua_gettop(L);
	if (luaL_loadstring(L, m_fileString.c_str()) != LUA_OK)
	{
		Logging::Log(Logging::SCRIPTING, Logging::CRITICAL_PRIORITY, "Failed to load Lua file \"", filePath, "\"\n. Lua says: ", lua_tostring(L, -1));
		Assert(!"Failed to load lua string. See logfile.");
	}
	lua_settop(L, currentStack);

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

void ScriptResource::ReloadData(const char *filePath)
{
	// Reloading is the same as loading.
	LoadData(filePath);
}

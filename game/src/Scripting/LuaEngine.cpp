/*
FILE: LuaEngine.cpp
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/

#include "Universal.h"
#include "LuaEngine.h"
#include "lua.hpp"
#include "ScriptingUtil.h"
#include "LuaRegistration.h"
#include "LuaSetup.h"

namespace
{

	void SetupEnvironmentTable(lua_State * L)
	{
		lua_newtable(L);
		lua_setfield(L, LUA_REGISTRYINDEX, SCRIPT_ENVIRONMENT_TABLE);
	}

	// Initializes a new Lua state and sets up data structures we may need.
	lua_State *InitNewLuaState()
	{
		// Set the custom lua function to redirect print to the console.
		SetLuaWriteFunction([](const char *str, size_t len) {Logging::Log(Logging::SCRIPTING, Logging::MEDIUM_PRIORITY, str); });
		SetLuaWriteLineFunction([]() {});

		lua_State *L = luaL_newstate();
		luaL_openlibs(L);

		SetupLua(L);

		SetupEnvironmentTable(L);

		RegisterLua(L);

		return L;
	}
}

lua_State *GetGlobalLuaState()
{
	static lua_State *L = InitNewLuaState();

	return L;
}

void RunLuaCommand(const char *commandString)
{
	luaL_dostring(GetGlobalLuaState(), commandString);
}

void RunLuaCommand(const std::string& commandString)
{
	RunLuaCommand(commandString.c_str());
}



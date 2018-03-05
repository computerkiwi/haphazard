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

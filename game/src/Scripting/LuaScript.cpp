/*
FILE: LuaScript.cpp
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/

#include "Universal.h"
#include "LuaScript.h"
#include "ScriptingUtil.h"
#include "LuaEngine.h"

#include <iostream>



LuaScript::LuaScript(const char * filename) : m_L(GetGlobalLuaState())
{
	// Create a table representing the script's environment.
	lua_newtable(m_L);

	// STACK: EnvTable

	// Create a metatable for the environment.
	lua_newtable(m_L);
	// Stick _G into the metatable as the index.
	lua_getglobal(m_L, "_G");
	lua_setfield(m_L, -2, "__index");
	// Attach the metatable.
	lua_setmetatable(m_L, -2);

	// STACK: EnvTable

	// Load the function and set its environment upvalue.
	int result = luaL_loadfile(m_L, filename);
	if (result != 0)
	{
		Logging::Log(Logging::CORE, Logging::HIGH_PRIORITY, "Lua couldn't load script file: ", lua_tostring(m_L, -1));
	}
	lua_pushvalue(m_L, -2);
	LuaSetUpValue(m_L, "_ENV", -2);
	
	// STACK: EnvTable, FileFunc

	// Get the script environment table and register the environment in it.
	lua_getfield(m_L, LUA_REGISTRYINDEX, SCRIPT_ENVIRONMENT_TABLE);
	assert(lua_istable(m_L, -1));
	lua_pushvalue(m_L, -3);
	m_environmentID = luaL_ref(m_L, -2);
	
	// STACK: EnvTable, FileFunc, ScriptEnvironmentTables

	// Clear the spare Tables
	lua_pop(m_L, 1);
	lua_remove(m_L, -2);

	// STACK: FileFunc

	// Run the file in the environment.
	result = lua_pcall(m_L, 0, 0, 0);
	if (result != 0)
	{
		std::cout << lua_tostring(m_L, -1) << std::endl;
	}
	// STACK: *EMPTY*
}

void LuaScript::RunFunction(const char *functionName, int args, int returns)
{
	GetScriptEnvironment();

	// Get the function out of the environment and clear the environment from the stack.
	lua_getfield(m_L, -1, functionName);
	lua_remove(m_L, -2);

	// Put the function under the arguments on the stack.
	lua_insert(m_L, -1 - args);

	int result = lua_pcall(m_L, args, returns, 0);
	if (result != 0)
	{
		std::cout << lua_tostring(m_L, -1) << std::endl;
	}
}

void LuaScript::GetScriptEnvironment()
{
	lua_getfield(m_L, LUA_REGISTRYINDEX, SCRIPT_ENVIRONMENT_TABLE);
	lua_rawgeti(m_L, -1, m_environmentID);
	lua_remove(m_L, -2);
}

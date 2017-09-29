/*
FILE: ScriptingUtil.h
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "lua.hpp"

// Name of the table in the Lua registry where we're storing script enviroments
#define SCRIPT_ENVIRONMENT_TABLE "HAPHAZARD_LUA_SCRIPT_ENVIRONMENT_TABLE"

// Sets the upvalue with a given name of the element at the given index on the Lua stack.
// If no upvalue with that name exists, does nothing.
// Pops the top value from the stack.
void LuaSetUpValue(lua_State *L, const char *upValueName, int index);

// TODO[Kieran]: Put this somewhere better.
inline void SetupEnvironmentTable(lua_State *L)
{
	lua_newtable(L);
	lua_setfield(L, LUA_REGISTRYINDEX, SCRIPT_ENVIRONMENT_TABLE);
}

// Nice Lua debugging stuff.
__declspec(dllexport) void Lua_PrintTable(lua_State *L, int index);
__declspec(dllexport) void PrintIndent();
__declspec(dllexport) void Lua_PrintIndex(lua_State *L, int index);
__declspec(dllexport) void Lua_PrintTable(lua_State *L, int index);
__declspec(dllexport) void Lua_stackDump(lua_State *L);
__declspec(dllexport) void Lua_stackTrace(lua_State *L);
void LuaStackTrace(lua_State* L);
void LuaStackDump(lua_State* L);


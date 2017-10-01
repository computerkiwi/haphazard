/*
FILE: LuaRegistration.h
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

struct lua_State;

// Temporary function for manually registering Lua types. Will make this more automatic later.
void RegisterLua(lua_State *L);

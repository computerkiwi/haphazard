/*
FILE: LuaScript.h
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "lua.hpp"

class LuaScript
{
public:
	LuaScript(lua_State *L, const char *filename);

	void RunFunction(const char *functionName, int args, int returns);

private:
	int m_environmentID;
	lua_State *m_L;
};

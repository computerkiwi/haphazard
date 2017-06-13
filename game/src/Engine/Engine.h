/*
FILE: Engine.h
PRIMARY AUTHOR: Sweet

Engine class data, no work really in here yet.

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <LuaBridge.h>

class Engine
{
public:
	void mainLoop() const
	{
		
	}

	void Update(float dt)
	{
		
	}

	float Dt() const;
	lua_State * GetLua() const;

private:
	float m_dt = 0.0f;
	lua_State * L = luaL_newstate();

};

void RegisterComponents(lua_State * L);


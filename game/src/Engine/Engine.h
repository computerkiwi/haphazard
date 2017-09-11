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
#include "GameObjectSystem/GameSpace.h"

class Engine
{
public:
	Engine();

	void MainLoop();

	void Update();

	float Dt() const;
	lua_State * GetLua();



private:
	float CalculateDt();

	float m_dt = 0.0f;
	bool m_running = true;
	lua_State * L = luaL_newstate();

	GameSpace m_space;
};

void RegisterComponents(lua_State * L);


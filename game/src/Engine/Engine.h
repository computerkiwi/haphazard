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
#include "../Imgui/Editor.h"

struct GLFWwindow;

class Engine
{
public:
	Engine();

	void MainLoop();

	void Update();

	float Dt() const;
	lua_State * GetLua();

	GameSpace *GetSpace() { return &m_space; }

	Editor *GetEditor() { return &m_editor; }

private:
	float CalculateDt();

	float m_dt = 0.0f;
	bool m_running = true;
	lua_State * L = luaL_newstate();
	GLFWwindow *m_window;
	GameSpace m_space;
	Editor   m_editor;
};

void RegisterComponents(lua_State * L);


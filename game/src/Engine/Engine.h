/*
FILE: Engine.h
PRIMARY AUTHOR: Sweet

Engine class data, no work really in here yet.

Copyright � 2017 DigiPen (USA) Corporation.
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

class Init_EnginePointer
{
public:
	Init_EnginePointer::Init_EnginePointer(Engine *e);
};

class Engine
{
public:
	Engine();

	void LoggingInit();

	void MainLoop();

	void Update();

	float Dt() const;
	lua_State * GetLua();

	GameSpaceManagerID *GetSpaceManager() { return &m_spaces; }
	GameSpace *GetSpace(std::size_t index) { return m_spaces[index]; }

	Editor *GetEditor() { return &m_editor; }

	void Exit() { m_running = false; }

private:
	float CalculateDt();

	Init_EnginePointer m_init;

	float m_dt = 0.0f;
	bool m_running = true;

	GLFWwindow *m_window;
	GameSpaceManagerID m_spaces;
	Editor   m_editor;
};


/*
FILE: Engine.cpp
PRIMARY AUTHOR: Sweet

Engine class data, no work really in here yet.

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Engine.h"


void Engine::MainLoop()
{
	while (m_running)
	{
		Update();
	}
}



void Engine::Update()
{
	m_dt = CalculateDt();
}


float Engine::CalculateDt()
{
	return (1 / 60.0f);
}



float Engine::Dt() const
{
	return m_dt;
}


lua_State * Engine::GetLua()
{
	return L;
}




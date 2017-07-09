/*
FILE: Engine.cpp
PRIMARY AUTHOR: Sweet

Engine class data, no work really in here yet.

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Engine.h"


float Engine::Dt() const
{
	return m_dt;
}


lua_State * Engine::GetLua() const
{
	return L;
}




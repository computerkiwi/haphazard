/*
FILE: LuaScript.h
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Universal.h"

#include <vector>

#include "LuaScript.h"
struct ScriptComponent
{
	ScriptComponent()
	{
	}

	ScriptComponent(const LuaScript& script)
	{
		scripts.push_back(script);
	}

	std::vector<LuaScript> scripts;

	META_REGISTER(ScriptComponent)
	{
		META_DefineType(ScriptComponent);
	}
};

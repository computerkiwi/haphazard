#include "ScriptSystem.h"
/*
FILE: ScriptSystem.cpp
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/

#include "lua.hpp"
#include "LuaBridge.h"

#include "ScriptSystem.h"
#include "ScriptComponent.h"
#include "ScriptingUtil.h"

void ScriptSystem::Init()
{

}

void ScriptSystem::Update(float dt)
{
	ComponentMap<ScriptComponent> *scripts = GetGameSpace()->GetComponentMap<ScriptComponent>();

	for (ComponentHandle<ScriptComponent> scriptComp : *scripts)
	{
		for (LuaScript script : scriptComp->scripts)
		{
			script.RunFunction("update", 0, 0);
		}
	}
}

std::size_t ScriptSystem::DefaultPriority()
{
	return 0;
}


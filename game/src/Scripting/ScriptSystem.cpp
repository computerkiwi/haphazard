#include "ScriptSystem.h"
/*
FILE: ScriptSystem.cpp
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/

#include "lua.hpp"
#include "LuaBridge.h"
#include "LuaEngine.h"

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
			luabridge::push(script.GetLuaState(), scriptComp.GetGameObject());
			luabridge::push(script.GetLuaState(), dt);
			script.RunFunction("update", 2, 0);
		}
	}
}

std::size_t ScriptSystem::DefaultPriority()
{
	return 0;
}


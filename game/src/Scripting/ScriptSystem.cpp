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
#include "GameObjectSystem\GameObject.h"

void ScriptSystem::Init()
{

}

void ScriptSystem::Update(float dt)
{
	ComponentMap<ScriptComponent> *scripts = GetGameSpace()->GetComponentMap<ScriptComponent>();

	// Call start on everything that needs it.
	for (ComponentHandle<ScriptComponent> scriptComp : *scripts)
	{
		if (scriptComp.IsActive())
		{
			for (LuaScript& script : scriptComp->scripts)
			{
				script.TryStartCall();
			}
		}
	}

	// Call update on everything that needs it.
	for (ComponentHandle<ScriptComponent> scriptComp : *scripts)
	{
		if (scriptComp.IsActive())
		{
			for (LuaScript& script : scriptComp->scripts)
			{
				luabridge::push(script.GetLuaState(), dt);
				script.RunFunction("Update", 1, 0);
			}
		}
	}
}

std::size_t ScriptSystem::DefaultPriority()
{
	return 0;
}

SystemBase *ScriptSystem::NewDuplicate()
{
	return new ScriptSystem(*this);
}


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

bool ScriptSystem::shouldReloadAllScripts = false;

void ScriptSystem::Init()
{

}

void ScriptSystem::Update(float dt)
{
	ComponentMap<ScriptComponent> *scripts = GetGameSpace()->GetComponentMap<ScriptComponent>();

	// Reload all the scripts if we were told to.
	if (shouldReloadAllScripts)
	{
		for (ComponentHandle<ScriptComponent> scriptComp : *scripts)
		{
			for (LuaScript& script : scriptComp->scripts)
			{
				script.Reload();
			}
		}

		shouldReloadAllScripts = false;
	}

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

	// Call EarlyUpdate on everything that needs it.
	for (ComponentHandle<ScriptComponent> scriptComp : *scripts)
	{
		if (scriptComp.IsActive())
		{
			for (LuaScript& script : scriptComp->scripts)
			{
				luabridge::push(script.GetLuaState(), dt);
				script.RunFunction("EarlyUpdate", 1, 0);
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

	// Call LateUpdate on everything that needs it.
	for (ComponentHandle<ScriptComponent> scriptComp : *scripts)
	{
		if (scriptComp.IsActive())
		{
			for (LuaScript& script : scriptComp->scripts)
			{
				luabridge::push(script.GetLuaState(), dt);
				script.RunFunction("LateUpdate", 1, 0);
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

void ScriptSystem::ReloadAll()
{
	shouldReloadAllScripts = true;
}


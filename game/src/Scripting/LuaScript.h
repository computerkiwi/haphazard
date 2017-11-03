/*
FILE: LuaScript.h
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "lua.hpp"
#include "LuaBridge.h"
#include "GameObjectSystem/GameObject.h"
#include "ScriptResource.h"

class LuaScript
{
public:
	LuaScript();

	LuaScript(Resource *resource, GameObject thisObj);

	void RunFunction(const char *functionName, int args, int returns);

	lua_State *GetLuaState() const { return m_L; }

	template <typename T>
	void AddVariable(const T& var, const char *name)
	{
		GetScriptEnvironment();
		luabridge::push(m_L, var);
		lua_setfield(m_L, -2, name);
		lua_pop(m_L, 1);
	}

private:
	// Puts the environment table for this script on the Lua stack.
	void GetScriptEnvironment();

	void SetScriptResource(Resource *resource);

	void SetResourceID(ResourceID id);

	ResourceID GetResourceID() const
	{
		return m_resID;
	}

	void UpdateThisObject();

	GameObject m_thisObj;
	ResourceID m_resID;
	int m_environmentID;
	lua_State *m_L;

	void SetThisObject(GameObject obj)
	{
		m_thisObj = obj;
		UpdateThisObject();
	}

	GameObject GetThisObject() const
	{
		return m_thisObj;
	}



	META_REGISTER(LuaScript)
	{
		META_DefineType(LuaScript);

		META_DefineGetterSetter(LuaScript, GameObject, GetThisObject, SetThisObject, "thisObject");
		META_DefineGetterSetter(LuaScript, ResourceID, GetResourceID, SetResourceID, "resourceID");
	}
};

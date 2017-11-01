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

	ResourceID m_id;
	int m_environmentID;
	lua_State *m_L;

	META_REGISTER(LuaScript)
	{
		META_DefineType(LuaScript);

		// TODO: Add functions for getting and setting script resource and "this" object.
	}
};

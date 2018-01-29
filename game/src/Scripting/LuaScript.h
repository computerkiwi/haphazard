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
	static bool currentlyRunningScript;
	static std::string currentFileName;

	LuaScript();
	~LuaScript();
	LuaScript(const LuaScript& other);
	LuaScript(LuaScript&& other);
	LuaScript& operator=(const LuaScript& other);
	LuaScript& operator=(LuaScript&& other);

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

	void SetScriptResource(Resource *resource);

	void SetResourceID(ResourceID id);

	// Gets a variable. First in the pair is whether a variable was succesfully retrieved. 
	std::pair<bool, meta::Any> GetVar(const char *varName);
	// Returns a vector with key-value pairs of all the vars in an environment table.
	std::vector<std::pair<std::string, meta::Any>> GetAllVars();
	// Sets a var in the environment table.
	void SetVar(const char *varName, meta::Any& value);

	ResourceID GetResourceID() const
	{
		return m_resID;
	}


	void SetThisObject(GameObject obj)
	{
		m_thisObj = obj;
		UpdateThisObject();
	}

	GameObject GetThisObject() const
	{
		return m_thisObj;
	}

	// Returns the environment table for the script as a lua_CFunction.
	int GetScriptEnvironmentLua(lua_State *L)
	{
		// Get the environment function and tell lua we're returning one thing.
		GetScriptEnvironment();
		return 1;
	}

	// Calls the start function on the script if it's needed.
	void TryStartCall()
	{
		if (m_needsStartCall)
		{
			RunFunction("Start", 0, 0);
			m_needsStartCall = false;
		}
	}

	// Reloads the from the file, saving current variables.
	void Reload();

	// Resets to default variables from current file (calls reload).
	void Reset();

private:
	// Puts the environment table for this script on the Lua stack.
	void GetScriptEnvironment();

	void SetupEnvironment(const char *scriptString);

	void UpdateThisObject();

	bool m_needsStartCall = true;

	GameObject m_thisObj;
	ResourceID m_resID;
	int m_environmentID;
	lua_State *m_L;

	// Deletes the environment table, etc.
	void UnloadScript();

	// For meta. (Mostly deserialization)
	static void MetaSetObject(void *scriptPtr, GameObject_ID id)
	{
		LuaScript& script = *reinterpret_cast<LuaScript *>(scriptPtr);

		script.SetThisObject(id);
	}

	static rapidjson::Value LuaScriptSerializeFunction(const void *scriptPtr, rapidjson::Document::AllocatorType& allocator);

	static void LuaScriptDeserializeAssign(void *scriptPtr, rapidjson::Value& jsonScript);

	META_REGISTER(LuaScript)
	{
		META_DefineGetterSetter(LuaScript, GameObject, GetThisObject, SetThisObject, "thisObject");
		META_DefineGetterSetter(LuaScript, ResourceID, GetResourceID, SetResourceID, "resourceID");

		META_DefineSetGameObjectIDFunction(LuaScript, &MetaSetObject);

		META_DefineSerializeFunction(LuaScript, LuaScriptSerializeFunction);
		META_DefineDeserializeAssignFunction(LuaScript, LuaScriptDeserializeAssign);

		luabridge::getGlobalNamespace(GetGlobalLuaState()).beginClass<LuaScript>("LuaScript").addCFunction("GetScriptEnvironment", &GetScriptEnvironmentLua).endClass();
	}
};

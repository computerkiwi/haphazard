/*
FILE: LuaScript.cpp
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/

#include "Universal.h"
#include "Engine/Engine.h"
#include "LuaScript.h"
#include "ScriptingUtil.h"
#include "LuaEngine.h"

#include <iostream>

#define INVALID_ID 0

bool LuaScript::currentlyRunningScript = false;
std::string LuaScript::currentFileName;

LuaScript::LuaScript() : m_L(GetGlobalLuaState()), m_resID(INVALID_ID)
{
}

LuaScript::LuaScript(const LuaScript& other) : m_thisObj(other.m_thisObj), m_L(other.m_L)
{
	SetResourceID(other.m_resID);
}

LuaScript::LuaScript(Resource *resource, GameObject thisObj) : m_L(GetGlobalLuaState()), m_thisObj(thisObj), m_resID(resource->Id())
{
	SetScriptResource(resource);
}

void LuaScript::RunFunction(const char *functionName, int args, int returns)
{
	Assert(m_resID != INVALID_ID);

	GetScriptEnvironment();

	// Get the function out of the environment and clear the environment from the stack.
	lua_getfield(m_L, -1, functionName);
	lua_remove(m_L, -2);

	// If the function wasn't there don't bother.
	if (lua_isnil(m_L, -1))
	{
		lua_remove(m_L, -1);
		for (int i = 0; i < args; ++i)
		{
			lua_remove(m_L, -1);
		}
		return;
	}

	// Put the function under the arguments on the stack.
	lua_insert(m_L, -1 - args);

	// Set a flag that assert can check to see if an error came from a script.
	currentlyRunningScript = true;
	currentFileName = engine->GetResourceManager().Get(m_resID)->GetFilename();
	int result = lua_pcall(m_L, args, returns, 0);
	currentlyRunningScript = false;
	if (result != 0)
	{
		logger.SetNextChannel(Logging::Channel::SCRIPTING);
		logger << engine->GetResourceManager().Get(m_resID)->FileName() <<": " << lua_tostring(m_L, -1) << "\n";
	}
}

void LuaScript::GetScriptEnvironment()
{
	lua_getfield(m_L, LUA_REGISTRYINDEX, SCRIPT_ENVIRONMENT_TABLE);
	lua_rawgeti(m_L, -1, m_environmentID);
	lua_remove(m_L, -2);
}

void LuaScript::SetupEnvironment(const char *scriptString)
{
	// Create a table representing the script's environment.
	lua_newtable(m_L);

	// STACK: EnvTable

	// Create a metatable for the environment.
	lua_newtable(m_L);
	// Stick _G into the metatable as the index.
	lua_getglobal(m_L, "_G");
	lua_setfield(m_L, -2, "__index");
	// Attach the metatable.
	lua_setmetatable(m_L, -2);

	// STACK: EnvTable

	// Load the function and set its environment upvalue.
	int result = luaL_loadstring(m_L, scriptString);
	if (result != 0)
	{
		Logging::Log(Logging::CORE, Logging::HIGH_PRIORITY, "Lua couldn't load script file: ", lua_tostring(m_L, -1));
	}
	lua_pushvalue(m_L, -2);
	LuaSetUpValue(m_L, "_ENV", -2);

	// STACK: EnvTable, FileFunc

	// Get the script environment table and register the environment in it.
	lua_getfield(m_L, LUA_REGISTRYINDEX, SCRIPT_ENVIRONMENT_TABLE);
	Assert(lua_istable(m_L, -1));
	lua_pushvalue(m_L, -3);
	m_environmentID = luaL_ref(m_L, -2);

	// STACK: EnvTable, FileFunc, ScriptEnvironmentTables

	// Clear the spare Tables
	lua_pop(m_L, 1);
	lua_remove(m_L, -2);

	// STACK: FileFunc

	// Run the file in the environment.
	result = lua_pcall(m_L, 0, 0, 0);
	if (result != 0)
	{
		std::cout << lua_tostring(m_L, -1) << std::endl;
	}
	// STACK: *EMPTY*

	UpdateThisObject();
}

void LuaScript::SetScriptResource(Resource *resource)
{
	// Make sure we're getting a script resource.
	Assert(resource->GetResourceType() == ResourceType::SCRIPT);

	SetupEnvironment(reinterpret_cast<std::string *>(resource->Data())->c_str());
}

void LuaScript::SetResourceID(ResourceID id)
{
	m_resID = id;
	SetScriptResource(engine->GetResourceManager().Get(id));
}

// First result is success, second is the value if first is true.
// [-0, +0, -]
static std::pair<bool, meta::Any> LuaValToAny(lua_State *L, int index)
{	
	// Check if there's anything at all.
	if (lua_isnoneornil(L, index))
	{
		return std::make_pair(false, meta::Any(0));
	}

	// Check each Lua type.
	if (lua_isboolean(L, index))
	{
		return std::make_pair(true, meta::Any(static_cast<bool>(lua_toboolean(L, index))));
	}
	if (lua_type(L, index) == LUA_TSTRING)
	{
		return std::make_pair(true, meta::Any(std::string(lua_tostring(L, index))));
	}
	if (lua_isnumber(L, index))
	{
		return std::make_pair(true, meta::Any(lua_tonumber(L, index)));
	}

	// If we got here, we don't currently support the type.
	Logging::Log(Logging::SCRIPTING, Logging::MEDIUM_PRIORITY, "Attempted to get variable with unsupported type ", lua_typename(L, index), " from a script.");
	return std::make_pair(false, meta::Any(0));
}

std::pair<bool, meta::Any> LuaScript::GetVar(const char * varName)
{
	// Pull the requested var out of the environment table.
	GetScriptEnvironment();
	lua_getfield(m_L, -1, varName);

	// Convert it.
	std::pair<bool, meta::Any> result = LuaValToAny(m_L, -1);

	// Clean up the stack and return.
	lua_pop(m_L, 2);
	return result;
}

std::vector<std::pair<std::string, meta::Any>> LuaScript::GetAllVars()
{
	std::vector<std::pair<std::string, meta::Any>> outVec;

	GetScriptEnvironment();
	int envIndex = lua_absindex(m_L, -1);

	// First key.
	lua_pushnil(m_L);
	while (lua_next(m_L, envIndex) != 0)
	{
		// Key should be at -2.
		assert(lua_isstring(m_L, -2));

		// Get the value.
		std::pair<bool, meta::Any> valPair = LuaValToAny(m_L, -1);
		// Only add the value if it's valid/usable.
		if (valPair.first)
		{
			outVec.push_back(std::make_pair(std::string(lua_tostring(m_L, -2)), valPair.second));
		}
		
		// Clear the value so the index is at the top ready for lua_next.
		lua_pop(m_L, 1);
	}

	// Clean up the stack.
	lua_settop(m_L, envIndex - 1);
	return outVec;
}

void LuaScript::SetVar(const char * varName, meta::Any & value)
{
	GetScriptEnvironment();
	
	// Define so we don't write each number type individually.
	#define IF_NUMBER_TYPE(TYPE)                                          \
	if (value.GetType() == meta::GetTypePointer<TYPE>())                  \
	{																																			\
		lua_pushnumber(m_L, static_cast<lua_Number>(value.GetData<TYPE>()));\
	}

	IF_NUMBER_TYPE(float)
	else IF_NUMBER_TYPE(double)
	else IF_NUMBER_TYPE(int)
	else IF_NUMBER_TYPE(unsigned int)
	else IF_NUMBER_TYPE(long)
	else IF_NUMBER_TYPE(unsigned long)
	else IF_NUMBER_TYPE(long long)
	else IF_NUMBER_TYPE(unsigned long long)
	else IF_NUMBER_TYPE(short)
	else IF_NUMBER_TYPE(unsigned short)
	else IF_NUMBER_TYPE(char)
	else IF_NUMBER_TYPE(unsigned char)
	else IF_NUMBER_TYPE(signed char)
	else if (value.GetType() == meta::GetTypePointer<std::string>())
	{
		lua_pushstring(m_L, value.GetData<std::string>().c_str());
	}
	else if (value.GetType() == meta::GetTypePointer<bool>())
	{
		lua_pushboolean(m_L, value.GetData<bool>());
	}
	else
	{
		// Only pop the environment table if we couldn't push a value.
		Logging::Log(Logging::SCRIPTING, Logging::HIGH_PRIORITY, "Attempted to set variable \"", varName, "\" with unsupported type ", value.GetType()->GetName(), " on a script.");
		lua_pop(m_L, 1);
		return;
	}

	lua_setfield(m_L, -2, varName);
}

void LuaScript::UpdateThisObject()
{
	if (!m_thisObj.IsValid())
	{
		return;
	}
	if (m_resID == INVALID_ID)
	{
		return;
	}

	GetScriptEnvironment();

	// Add the gameobject as "this"
	luabridge::push(m_L, m_thisObj);
	lua_setfield(m_L, -2, "this");

	// Remove the environment table.
	lua_pop(m_L, 1);
}

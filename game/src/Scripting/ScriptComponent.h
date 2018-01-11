/*
FILE: ScriptComponent.h
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include <vector>
#include "Universal.h"
#include "meta\meta.h"
#include "Util\Assert.h"

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

	ScriptComponent(const ScriptComponent& other, const GameObject& newThisObject) : scripts(other.scripts)
	{
		for (LuaScript& script : scripts)
		{
			script.SetThisObject(newThisObject);
		}
	}

	void CallCollision(GameObject collidedObj)
	{
		for (auto& script : scripts)
		{
			luabridge::push(script.GetLuaState(), collidedObj);
			script.RunFunction("OnCollisionEnter", 1, 0);
		}
	}

	std::vector<LuaScript> scripts;

	// Returns a script with the given filename on the component, or nullptr if it doesn't exist.
	LuaScript *GetScriptByFilename(const char *fileName)
	{
		// Go through the scripts until we find the one with the proper filename.
		for (auto& script : scripts)
		{
			if (script.GetResourceID() == Resource::FilenameToID(fileName))
			{
				return &script;
			}
		}

		// If we got here we didn't find the script.
		return nullptr;
	}

	static rapidjson::Value ScriptComponentSerializeFunction(const void *scriptPtr, rapidjson::Document::AllocatorType& allocator)
	{
		// Const cast away is fine because we're not really changing anything.
		ScriptComponent& scriptComponent = *reinterpret_cast<ScriptComponent *>(const_cast<void *>(scriptPtr));

		// Setup the object to store the scripts in.
		rapidjson::Value scriptArray;
		scriptArray.SetArray();

		for (LuaScript& script : scriptComponent.scripts)
		{
			scriptArray.PushBack(meta::Serialize(script, allocator), allocator);
		}

		return scriptArray;
	}

	static void ScriptComponentDeserializeAssign(void *scriptPtr, rapidjson::Value& jsonScripts)
	{
		// Get the engine.
		ScriptComponent& scriptComponent = *reinterpret_cast<ScriptComponent *>(scriptPtr);

		// Get rid of all the scripts we have.
		scriptComponent.scripts.clear();

		// We should be passed the array of scripts.
		Assert(jsonScripts.IsArray());
		for (rapidjson::Value& jsonScript : jsonScripts.GetArray())
		{
			meta::Any script(jsonScript);
			scriptComponent.scripts.push_back(script.GetData<LuaScript>());
		}
	}

private:

	// For meta. (Mostly deserialization)
	static void MetaSetObject(void *scriptPtr, GameObject_ID id)
	{
		ScriptComponent& script = *reinterpret_cast<ScriptComponent *>(scriptPtr);

		for (LuaScript& script : script.scripts)
		{
			script.SetThisObject(id);
		}

	}

	META_REGISTER(ScriptComponent)
	{
		META_DefineSerializeFunction(ScriptComponent, ScriptComponentSerializeFunction);
		META_DefineDeserializeAssignFunction(ScriptComponent, ScriptComponentDeserializeAssign);

		META_DefineFunction(ScriptComponent, GetScriptByFilename, "GetScriptByFilename");

		META_DefineSetGameObjectIDFunction(ScriptComponent, MetaSetObject);
	}
};

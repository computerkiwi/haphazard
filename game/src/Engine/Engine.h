/*
FILE: Engine.h
PRIMARY AUTHOR: Sweet

Engine class data, no work really in here yet.

Copyright � 2017 DigiPen (USA) Corporation.
*/
#pragma once

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <LuaBridge.h>
#include "GameObjectSystem/GameSpace.h"
#include "../Imgui/Editor.h"

struct GLFWwindow;

class Init_EnginePointer
{
public:
	Init_EnginePointer::Init_EnginePointer(Engine *e);
};

class Engine
{
public:
	Engine();

	void LoggingInit();

	void MainLoop();

	void Update();

	float Dt() const;
	lua_State * GetLua();

	GameSpaceManagerID *GetSpaceManager() { return &m_spaces; }
	GameSpace *GetSpace(std::size_t index) { return m_spaces[index]; }

	Editor *GetEditor() { return &m_editor; }

	void Exit() { m_running = false; }

	std::string StringSave();
	void FileSave(const char *fileName);

	void StringLoad(const char *jsonString);
	void FileLoad(const char *fileName);

private:
	float CalculateDt();

	Init_EnginePointer m_init;

	float m_dt = 0.0f;
	bool m_running = true;

	GLFWwindow *m_window;
	GameSpaceManagerID m_spaces;
	Editor   m_editor;

	static rapidjson::Value EngineSerializeFunction(const void *enginePtr, rapidjson::Document::AllocatorType& allocator)
	{
		// Const cast away is fine because we're not really changing anything.
		Engine& engine = *reinterpret_cast<Engine *>(const_cast<void *>(enginePtr));

		// Setup the object to store the engine info in.
		rapidjson::Value spaceArray;
		spaceArray.SetArray();

		for (GameSpace& space : engine.m_spaces.GetSpaces())
		{
			spaceArray.PushBack(meta::Serialize(space, allocator), allocator);
		}

		return spaceArray;
	}

	static void EngineDeserializeAssign(void *enginePtr, rapidjson::Value& jsonEngine)
	{
		// Get the engine.
		Engine& engine = *reinterpret_cast<Engine *>(enginePtr);

		// Get rid of all the gamespaces we have.
		engine.m_spaces.ClearSpaces();

		// We should be passed the array of spaces.
		assert(jsonEngine.IsArray());
		for (rapidjson::Value& jsonSpace : jsonEngine.GetArray())
		{
			GameSpaceIndex index = engine.m_spaces.AddSpace();
			meta::DeserializeAssign(*engine.GetSpace(index), jsonSpace);
		}
	}

	META_REGISTER(Engine)
	{
		META_DefineType(Engine);
		META_DefineSerializeFunction(Engine, EngineSerializeFunction);
		META_DefineDeserializeAssignFunction(Engine, EngineDeserializeAssign);
	}
};


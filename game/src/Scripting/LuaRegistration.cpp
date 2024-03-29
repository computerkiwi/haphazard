/*
FILE: LuaRegistration.cpp
PRIMARY AUTHOR: Kiera Williams

Temporary file to manually register some stuff.

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "LuaRegistration.h"

#include "Engine/Engine.h"

#include "lua.hpp"
#include "LuaBridge.h"

#include "Input/Input.h"

#include "Engine/Physics/CollisionLayer.h"

#include "Audio/AudioEngine.h"

void ToggleFullscreen();

//-------
// Input
//-------
bool LuaIsPressed(int val)
{
	return Input::IsHeldDown(static_cast<Key>(val));
}

bool LuaIsTriggered(int val)
{
	return Input::IsPressed(static_cast<Key>(val));
}

bool LuaGamepadIsPressed(int player, int button)
{
  return Input::GamepadIsHeldDown(static_cast<PlayerID>(player), static_cast<GamepadButton>(button));
}

bool LuaGamepadIsTriggered(int player, int button)
{
	return Input::GamepadIsPressed(static_cast<PlayerID>(player), static_cast<GamepadButton>(button));
}

float LuaGamepadGetAxis(int player, int axis)
{
  return Input::GamepadGetAxis(static_cast<PlayerID>(player), static_cast<GamepadAxis>(axis));
}

glm::vec2 LuaMousePos()
{
	glm::vec2 vec;

	vec + vec;

	return Input::GetMousePos_World();
}

int LuaGamepadsConnected()
{
  return Input::GamepadsConnected();
}

void LuaSetPaused(bool paused)
{
	engine->SetPaused(paused);
}
bool LuaIsPaused()
{
	return engine->IsPaused();
}

const char *LuaCurrentLevel()
{
	return engine->GetCurrentLevel().c_str();
}

bool LuaEditorIsOpen()
{
	return engine->GetEditor()->GetEditorState().show;
}

void LuaQuitGame()
{
	engine->Exit();
}

void RegisterLua(lua_State * L)
{
  luabridge::getGlobalNamespace(L)
		.addFunction("IsHeld", &LuaIsPressed)
		.addFunction("OnPress", &LuaIsTriggered)
    .addFunction("GamepadIsHeld", &LuaGamepadIsPressed)
	  .addFunction("GamepadOnPress", &LuaGamepadIsTriggered)
    .addFunction("GamepadGetAxis", &LuaGamepadGetAxis)
    .addFunction("SetLayersColliding", CollisionLayer_SetLayersColliding)
    .addFunction("SetLayersNotColliding", CollisionLayer_SetLayersNotColliding)
    .addFunction("MouseToWorld", &Input::ScreenToWorld)
    .addFunction("ScreenToWorld", &Input::ScreenPercentToWorld)
    .addFunction("GamepadsConnected", &LuaGamepadsConnected)
		.addFunction("GetMousePos", &LuaMousePos)
		.addFunction("CurrentLevel", &LuaCurrentLevel)
		.addFunction("EditorIsOpen", &LuaEditorIsOpen)
		.addFunction("QuitGame", &LuaQuitGame)
		.addFunction("ToggleFullscreen", &ToggleFullscreen)

		.addFunction("PlaySound", Audio::PlaySound)
		.addFunction("PlayMusic", Audio::PlayMusic)
		.addFunction("ToggleMusic", Audio::ToggleMusic)
		.addFunction("ToggleSFX", Audio::ToggleSFX)
		.addFunction("SetQuietMusic", Audio::SetQuietMusic)
		.addFunction("GetMusic", Audio::GetMusic)
		.addFunction("SetPaused", &LuaSetPaused)
		.addFunction("IsPaused", &LuaIsPaused);
		
}

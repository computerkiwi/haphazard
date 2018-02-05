/*
FILE: LuaRegistration.cpp
PRIMARY AUTHOR: Kieran Williams

Temporary file to manually register some stuff.

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "LuaRegistration.h"

#include "lua.hpp"
#include "LuaBridge.h"

#include "Input/Input.h"

#include "Engine/Physics/CollisionLayer.h"

#include "Audio/AudioEngine.h"

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

		.addFunction("PlaySound", Audio::PlaySound);
		
}

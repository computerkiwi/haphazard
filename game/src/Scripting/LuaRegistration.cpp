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

bool LuaGamepadIsPressed(int player, int button)
{
  return Input::GamepadIsPressed(static_cast<PlayerID>(player), static_cast<GamepadButton>(button));
}

float LuaGamepadGetAxis(int player, int axis)
{
  return Input::GamepadGetAxis(static_cast<PlayerID>(player), static_cast<GamepadAxis>(axis));
}


void RegisterLua(lua_State * L)
{
	luabridge::getGlobalNamespace(L)
		.addFunction("IsPressed", &LuaIsPressed)
    .addFunction("GamepadIsPressed", &LuaGamepadIsPressed)
    .addFunction("GamepadGetAxis", &LuaGamepadGetAxis)
		.addFunction("SetLayersColliding", CollisionLayer_SetLayersColliding)
		.addFunction("SetLayersNotColliding", CollisionLayer_SetLayersNotColliding)

		.addFunction("PlaySound", Audio::PlaySound);
		
}

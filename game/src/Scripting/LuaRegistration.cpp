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

//-------
// Input
//-------
bool LuaIsPressed(int val)
{
	return Input::IsHeldDown(static_cast<Key>(val));
}


void RegisterLua(lua_State * L)
{
	luabridge::getGlobalNamespace(L)
		.addFunction("IsPressed", &LuaIsPressed)

		.addFunction("SetLayersColliding", CollisionLayer_SetLayersColliding)
		.addFunction("SetLayersNotColliding", CollisionLayer_SetLayersNotColliding);
		
}

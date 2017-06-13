/*
FILE: Engine.h
PRIMARY AUTHOR: Sweet

Engine class data, no work really in here yet.

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once
#include "../../GameObjects/GameObject.h"
#include "../../GameObjects/Components/Components.h"



typedef void(*GameObject_Constructor)(GameObject_Space &);

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include <LuaBridge.h>


void RegisterComponents(lua_State * L)
{
	luabridge::getGlobalNamespace(L)
		.beginClass<GameObject>("GameObject")
			.addConstructor<GameObject_Constructor>()
			.addFunction("GetSprite", &GameObject::GetComponent<Sprite>)
			.addFunction("GetTransform", &GameObject::GetComponent<Transform>)
		.endClass();
}


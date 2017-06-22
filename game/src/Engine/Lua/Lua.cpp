/*
FILE: Lua.cpp
PRIMARY AUTHOR: Sweet

Test file for LuaBridge in engine

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once
#include "../../GameObjects/GameObject.h"
#include "../../GameObjects/Components/Components.h"


#include "../Engine.h"



void RegisterComponents(lua_State * L)
{
	luabridge::getGlobalNamespace(L)

		.beginClass<GameObject_Space>("GameObject_Space")
			.addConstructor<void(*)()>()
			.addFunction("RegisterSprite", &GameObject_Space::Register<Sprite>)
		.endClass()

		.beginClass<Sprite>("Sprite")
			.addConstructor<void(*)()>()
			.addFunction("print", &Sprite::print)
		.endClass()

		.beginClass<Script>("Script")
			.addFunction("GetSprite", &Script::GetComponent<Sprite>)
			.addFunction("SetSprite", &Script::SetComponent<Sprite>)

			.addFunction("GetTransform", &Script::GetComponent<Transform>)
			.addFunction("SetTransform", &Script::SetComponent<Transform>)

			.addFunction("GetRigidBody2D", &Script::GetComponent<RigidBody2D>)
			.addFunction("SetRigidBody2D", &Script::SetComponent<RigidBody2D>)

			.addFunction("GetScript", &Script::GetComponent<Script>)
			.addFunction("SetScript", &Script::SetComponent<Script>)

			.addFunction("GetCollider2D", &Script::GetComponent<Collider2D>)
			.addFunction("SetCollider2D", &Script::SetComponent<Collider2D>)

			.addFunction("id", &GameObject::id)

			.addFunction("print", &GameObject::print)
		.endClass();
}


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
		.beginClass<GameObject>("GameObject")
			.addConstructor<void(*)(GameObject_Space *)>()
			.addFunction("GetSprite", &GameObject::GetComponent<Sprite>)
			.addFunction("SetSprite", &GameObject::SetComponent<Sprite, 0>)

			.addFunction("GetTransform", &GameObject::GetComponent<Transform>)
			.addFunction("SetSprite", &GameObject::SetComponent<Transform, 0>)

			.addFunction("GetRigidBody2D", &GameObject::GetComponent<RigidBody2D>)
			.addFunction("SetSprite", &GameObject::SetComponent<RigidBody2D, 0>)

			.addFunction("GetScript", &GameObject::GetComponent<Script>)
			.addFunction("SetScript", &GameObject::SetComponent<Script, 0>)

			.addFunction("GetCollider2D", &GameObject::GetComponent<Collider2D>)
			.addFunction("SetCollider2D", &GameObject::SetComponent<Collider2D, 0>)

			.addFunction("id", &GameObject::id)

			.addFunction("print", &GameObject::print)

		.endClass();
}


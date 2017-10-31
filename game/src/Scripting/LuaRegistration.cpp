/*
FILE: LuaRegistration.cpp
PRIMARY AUTHOR: Kieran Williams

Temporary file to manually register some stuff.

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "LuaRegistration.h"

#include "lua.hpp"
#include "LuaBridge.h"

#include "glm\glm.hpp"

#include "GameObjectSystem/TransformComponent.h"
#include "Engine/Physics/RigidBody.h"
#include "GameObjectSystem/GameObject.h"
#include "GameObjectSystem/Component.h"
#include "Input/Input.h"

//------------
// GameObject
//------------
TransformComponent *GameObjectGetTransform(GameObject &gObject)
{
	return gObject.GetComponent<TransformComponent>().Get();
}
RigidBodyComponent *GameObjectGetRigidBody(GameObject &gObject)
{
	return gObject.GetComponent<RigidBodyComponent>().Get();
}


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

		.beginClass<GameObject>("GameObject")
		.addStaticFunction("GetTransform", &GameObjectGetTransform)
		.addStaticFunction("GetRigidBody", &GameObjectGetRigidBody)
		.endClass()


		.addFunction("IsPressed", &LuaIsPressed);
		
}

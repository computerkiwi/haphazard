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

//-----------
// TRANSFORM
//-----------

glm::vec2 TransformGetPosition(const TransformComponent *transform)
{
	return transform->GetPosition();
}
void TransformSetPosition(TransformComponent *transform, glm::vec3 vector)
{
	transform->SetPosition(vector);
}

glm::vec3 TransformGetScale(const TransformComponent *transform)
{
	return transform->GetScale();
}
void TransformSetScale(TransformComponent *transform, glm::vec3 scale)
{
	transform->SetScale(scale);
}

float TransformGetRotation(const TransformComponent *transform)
{
	return transform->GetRotation();
}
void TransformSetRotation(TransformComponent *transform, float rotation)
{
	transform->SetRotation(rotation);
}

//-----------
// RigidBody
//-----------

glm::vec3 RigidBodyGetVelocity(const RigidBodyComponent *rigidBody)
{
	return rigidBody->Velocity();
}
void RigidBodySetVelocity(RigidBodyComponent *rigidBody, glm::vec3 vector)
{
	rigidBody->SetVelocity(vector);
}

glm::vec3 RigidBodyGetAcceleration(const RigidBodyComponent *rigidBody)
{
	return rigidBody->Acceleration();
}
void RigidBodySetAcceleration(RigidBodyComponent *rigidBody, glm::vec3 vector)
{
	rigidBody->SetAcceleration(vector);
}

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
	return Input::IsPressed(val);
}


void RegisterLua(lua_State * L)
{
	luabridge::getGlobalNamespace(L)

		.beginClass<glm::vec3>("vec3")
		.addData("x", &glm::vec3::x, true)
		.addData("y", &glm::vec3::y, true)
		.addData("z", &glm::vec3::z, true)
		.endClass()

		.beginClass<TransformComponent>("TransformComponent")
		.addProperty("position", &TransformGetPosition, &TransformSetPosition)
		.addProperty("scale", &TransformGetScale, &TransformSetScale)
		.addProperty("rotation", &TransformGetRotation, &TransformSetRotation)
		.endClass()

		.beginClass<RigidBodyComponent>("RigidBodyComponent")
		.addProperty("velocity", &RigidBodyGetVelocity, &RigidBodySetVelocity)
		.addProperty("acceleration", &RigidBodyGetAcceleration, &RigidBodySetAcceleration)
		.endClass()

		.beginClass<GameObject>("GameObject")
		.addStaticFunction("GetTransform", &GameObjectGetTransform)
		.addStaticFunction("GetRigidBody", &GameObjectGetRigidBody)
		.endClass()

		.addFunction("IsPressed", &LuaIsPressed);
		
}

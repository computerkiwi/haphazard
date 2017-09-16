/*
FILE: PhysicsSystem.cpp
PRIMARY AUTHOR: Brett Schiff

Handles Movement, Colliders, Collision Detection, and Collision Resolution

Copyright © 2017 DigiPen (USA) Corporation.
*/
#include "PhysicsSystem.h"
#include "RigidBody.h"
#include "Collider2D.h"





void PhysicsSystem::Init()
{

}

void PhysicsSystem::Update(float dt)
{
	// get all rigid bodies
	ComponentMap<RigidBodyComponent> *rigidBodies = GetGameSpace()->GetComponentMap<RigidBodyComponent>();

	for (auto tRigidBodyHandle : *rigidBodies)
	{
		// get the transform from the same gameobject, and leave the loop if it isn't valid
		ComponentHandle<TransformComponent> transform = tRigidBodyHandle.GetSiblingComponent<TransformComponent>();
		if (!transform.IsValid())
		{
			continue;
		}

		// update position, velocity, and acceleration using stored values
		transform->Position() += tRigidBodyHandle->Velocity();
		tRigidBodyHandle->AddVelocity(tRigidBodyHandle->Acceleration());
		tRigidBodyHandle->AddVelocity(tRigidBodyHandle->QueuedForce());
	}
}
/*
FILE: RigidBody.cpp
PRIMARY AUTHOR: Brett Schiff

Velocity, Acceleration, and Mass

Copyright © 2017 DigiPen (USA) Corporation.
*/
#include <assert.h>
#include "RigidBody.h"


// getters
glm::vec3 RigidBodyComponent::Acceleration() const
{
	return acceleration_;
}

glm::vec3 RigidBodyComponent::Velocity() const
{
	return velocity_;
}

float RigidBodyComponent::Mass() const
{
	return inverseMass_;
}

// setters
void RigidBodyComponent::SetAcceleration(glm::vec3 newAcceleration)
{
	acceleration_ = newAcceleration;
}

void RigidBodyComponent::SetVelocity(glm::vec3 newVelocity)
{
	velocity_ = newVelocity;
}
void RigidBodyComponent::SetMass(float newMass)
{
	inverseMass_ = newMass;
}

// adjusters
void RigidBodyComponent::AddForce(glm::vec3 addForce)
{
	assert(inverseMass_ != 0 && "\nA force was added to an object with mass = 0 \n See the AddForce function in RigidBody.cpp\n");
	acceleration_ += (addForce * inverseMass_);
}

void RigidBodyComponent::AddAcceleration(glm::vec3 addAcceleration)
{
	acceleration_ += addAcceleration;
}

void RigidBodyComponent::AddVelocity(glm::vec3 addVelocity)
{
	velocity_ += addVelocity;
}

void RigidBodyComponent::AddMass(float addMass)
{
	inverseMass_ += ( 1 / addMass);
}

void RigidBodyComponent::Update(float dt, ComponentHandle<TransformComponent> transform)
{
	transform->Position() += velocity_;
	velocity_ += acceleration_;
}




void RigidBodySystem::Init()
{
	
}

void RigidBodySystem::Update(float dt)
{
	// get all rigid bodies
	ComponentMap<RigidBodyComponent> *rigidBodies = GetGameSpace()->GetComponentMap<RigidBodyComponent>();

	for (auto tRigidBodyHandle : *rigidBodies)
	{
		// get the transform from the same gameobject
		ComponentHandle<TransformComponent> transform = tRigidBodyHandle.GetSiblingComponent<TransformComponent>();
		if (!transform.IsValid())
		{
			tRigidBodyHandle->Update(dt, transform);
		}
	}
}
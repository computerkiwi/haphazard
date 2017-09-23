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
	return m_acceleration;
}

glm::vec3 RigidBodyComponent::Velocity() const
{
	return m_velocity;
}

float RigidBodyComponent::Mass() const
{
	return m_inverseMass;
}

glm::vec3 RigidBodyComponent::Gravity() const
{
	return m_gravity;
}


// setters
void RigidBodyComponent::SetAcceleration(glm::vec3 newAcceleration)
{
	m_acceleration = newAcceleration;
}

void RigidBodyComponent::SetVelocity(glm::vec3 newVelocity)
{
	m_velocity = newVelocity;
}
void RigidBodyComponent::SetMass(float newMass)
{
	m_inverseMass = newMass;
}

void RigidBodyComponent::SetGravity(glm::vec3 newGravity)
{
	m_gravity = newGravity;
}

// adjusters
void RigidBodyComponent::AddForce(glm::vec3 addForce)
{
	assert(m_inverseMass != 0 && "\nA force was added to an object with mass = 0 \n See the AddForce function in RigidBody.cpp\n");
	m_acceleration += (addForce * m_inverseMass);
}

void RigidBodyComponent::AddAcceleration(glm::vec3 addAcceleration)
{
	m_acceleration += addAcceleration;
}

void RigidBodyComponent::AddVelocity(glm::vec3 addVelocity)
{
	m_velocity += addVelocity;
}

void RigidBodyComponent::ScaleGravity(glm::vec3 gravityScalar)
{
	m_gravity *= gravityScalar;
}

void RigidBodyComponent::AddMass(float addMass)
{
	m_inverseMass += ( 1 / addMass);
}

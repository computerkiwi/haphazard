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

glm::vec3 RigidBodyComponent::QueuedForce() const
{
	return m_queuedForce;
}

float RigidBodyComponent::Mass() const
{
	return m_inverseMass;
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

// adjusters
void RigidBodyComponent::AddForce(glm::vec3 addForce)
{
	assert(m_inverseMass != 0 && "\nA force was added to an object with mass = 0 \n See the AddForce function in RigidBody.cpp\n");
	m_queuedForce += (addForce * m_inverseMass);
}

void RigidBodyComponent::AddAcceleration(glm::vec3 addAcceleration)
{
	m_acceleration += addAcceleration;
}

void RigidBodyComponent::AddVelocity(glm::vec3 addVelocity)
{
	m_velocity += addVelocity;
}

void RigidBodyComponent::AddMass(float addMass)
{
	m_inverseMass += ( 1 / addMass);
}

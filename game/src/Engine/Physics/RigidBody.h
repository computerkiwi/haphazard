/*
FILE: RigidBody.h
PRIMARY AUTHOR: Brett Schiff

Velocity, Acceleration, and Mass

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "glm/glm.hpp"
#include "../../GameObjectSystem/TransformComponent.h"
#include "../../GameObjectSystem/GameSpace.h"

class RigidBodyComponent
{
public:
	// constructors
	RigidBodyComponent(glm::vec3 acceleration = glm::vec3(0), glm::vec3 velocity = glm::vec3(0), float mass = 0, glm::vec3 queuedForce = glm::vec3(0,0,0)) : m_acceleration(acceleration), m_velocity(velocity), m_inverseMass(mass), m_queuedForce(queuedForce)
	{
	}

	// getters
	glm::vec3 Acceleration() const;
	glm::vec3 Velocity() const;
	glm::vec3 QueuedForce() const;
	float Mass() const;

	// setters
	void SetAcceleration(glm::vec3 newAcceleration);
	void SetVelocity(glm::vec3 newVelocity);
	void SetMass(float newMass);

	// methods
	void AddForce(glm::vec3 addForce);
	void AddAcceleration(glm::vec3 addAcceleration);
	void AddVelocity(glm::vec3 addVelocity);
	void AddMass(float addMass);

private:
	glm::vec3 m_acceleration;
	glm::vec3 m_velocity;
	glm::vec3 m_queuedForce;
	float m_inverseMass;
};
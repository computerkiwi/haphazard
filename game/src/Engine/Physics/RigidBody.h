/*
FILE: RigidBody.h
PRIMARY AUTHOR: Brett Schiff

Velocity, Acceleration, and Mass

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "glm/glm.hpp"
#include "GameObjectSystem\GameObject.h"
#include "../../GameObjectSystem/TransformComponent.h"
#include "../../GameObjectSystem/GameSpace.h"

#define DEFAULT_GRAVITY glm::vec3(0, -3, 0)

class RigidBodyComponent
{
public:
	// constructors
	RigidBodyComponent(glm::vec3 acceleration = glm::vec3(0), glm::vec3 velocity = glm::vec3(0), glm::vec3 gravity = DEFAULT_GRAVITY, float mass = 0) : m_acceleration(acceleration + gravity), m_velocity(velocity), m_gravity(gravity), m_inverseMass(mass)
	{
	}

	// getters
	glm::vec3 Acceleration() const;
	glm::vec3 Velocity() const;
	glm::vec3 Gravity() const;
	float Mass() const;

	// setters
	void SetAcceleration(glm::vec3 newAcceleration);
	void SetVelocity(glm::vec3 newVelocity);
	void SetGravity(glm::vec3 newGravity);
	void SetMass(float newMass);

	// methods
	void AddForce(glm::vec3 addForce);
	void AddAcceleration(glm::vec3 addAcceleration);
	void AddVelocity(glm::vec3 addVelocity);
	void ScaleGravity(glm::vec3 gravityScalar);
	void AddMass(float addMass);

private:
	friend void ImGui_RigidBody(RigidBodyComponent *rb, GameObject object, Editor * editor);
	glm::vec3 m_acceleration;
	glm::vec3 m_velocity;
	glm::vec3 m_gravity;
	float m_inverseMass;

	META_REGISTER(RigidBodyComponent)
	{
		META_DefineType(RigidBodyComponent);
		META_DefineType(glm::vec3);
		META_DefineType(float);

		META_DefineGetterSetter(RigidBodyComponent, glm::vec3, Acceleration, SetAcceleration, "acceleration");
		META_DefineGetterSetter(RigidBodyComponent, glm::vec3, Velocity, SetVelocity, "velocity");
		META_DefineGetterSetter(RigidBodyComponent, glm::vec3, Gravity, SetGravity, "gravity");
		META_DefineGetterSetter(RigidBodyComponent, float, Mass, SetMass, "mass");
	}
};
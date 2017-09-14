/*
FILE: RigidBody.h
PRIMARY AUTHOR: Brett Schiff

Velocity, Acceleration, and Mass

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "glm/glm.hpp"
#include "../../GameObjectSystem/GameSpace.h"
#include "../../GameObjectSystem/TransformComponent.h"

class RigidBodyComponent
{
public:
	// constructors
	RigidBodyComponent() : acceleration_(glm::vec3(0, 0, 0)), velocity_(glm::vec3(0, 0, 0)), inverseMass_(1.0f)
	{
	}

	RigidBodyComponent(glm::vec3 acceleration, glm::vec3 velocity, float mass) : acceleration_(acceleration), velocity_(velocity), inverseMass_(mass)
	{
	}

	// getters
	glm::vec3 Acceleration() const;
	glm::vec3 Velocity() const;
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

	void Update(float dt, ComponentHandle<TransformComponent> transform);

private:
	glm::vec3 acceleration_;
	glm::vec3 velocity_;
	float inverseMass_;
};

class RigidBodySystem : public SystemBase
{
	virtual void Init();

	// Called each frame.
	virtual void Update(float dt);

	// Simply returns the default priority for this system.
	virtual size_t DefaultPriority()
	{
		return 1;
	}
};

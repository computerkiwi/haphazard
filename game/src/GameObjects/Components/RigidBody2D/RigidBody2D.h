/*
FILE: Transform.h
PRIMARY AUTHOR: Sweet

Entry point into the program.
Currently just a place to play around in.

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "../Component.h"
#include <glm/vec2.hpp>


class RigidBody2D : public Component
{
public:
	
	RigidBody2D();
	explicit RigidBody2D(GameObject * parent);

	RigidBody2D(GameObject * parent, glm::vec2 & velocity, glm::vec2 & acceleration);
	RigidBody2D(GameObject * parent, glm::vec2 && velocity, glm::vec2 && acceleration);
	RigidBody2D(GameObject * parent, float angularVelocity, float mass);

	RigidBody2D(GameObject * parent, glm::vec2 & velocity, glm::vec2 & acceleration, float angularVelocity, float mass);
	RigidBody2D(GameObject * parent, glm::vec2 && velocity, glm::vec2 && acceleration, float angularVelocity, float mass);

	glm::vec2 GetVelocity() const;
	void SetVelocity(glm::vec2 & velocity);

	glm::vec2 GetAcceleration() const;
	void SetAcceleration(glm::vec2 & acceleration);

	float GetAngularVelocity() const;
	void SetAngularVelocity(float r_velocity);

	float GetMass() const;
	void SetMass(float mass);


private:
	glm::vec2   mVelocity;
	glm::vec2   mAcceleration;
	float       mAngularVel;
	float       mInverseMass;
};


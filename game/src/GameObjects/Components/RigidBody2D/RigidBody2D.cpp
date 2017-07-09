/*
FILE: RigidBody2D.cpp
PRIMARY AUTHOR: Sweet

RigidBody Function Bodies

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "RigidBody2D.h"


RigidBody2D::RigidBody2D() : RigidBody2D(nullptr, glm::vec2(), glm::vec2(), 0.0f, 1.0f)
{
}


RigidBody2D::RigidBody2D(GameObject * parent) : RigidBody2D(parent, glm::vec2(), glm::vec2(), 0.0f, 1.0f)
{
}



RigidBody2D::RigidBody2D(GameObject * parent, float angularVelocity, float mass)
	: RigidBody2D(parent, glm::vec2(), glm::vec2(), angularVelocity, mass)
{
}


RigidBody2D::RigidBody2D(GameObject * parent, glm::vec2 & velocity, glm::vec2 & acceleration)
	: RigidBody2D(parent, velocity, acceleration, 0.0f, 1.0f)
{
}


RigidBody2D::RigidBody2D(GameObject * parent, glm::vec2 && velocity, glm::vec2 && acceleration)
	: RigidBody2D(parent, velocity, acceleration, 0.0f, 1.0f)
{
}


RigidBody2D::RigidBody2D(GameObject * parent, glm::vec2 & velocity, glm::vec2 & acceleration, float angularVelocity, float mass)
	: Component(parent), mVelocity(velocity), mAcceleration(acceleration), mAngularVel(angularVelocity), mInverseMass(1 / mass)
{
}


RigidBody2D::RigidBody2D(GameObject * parent, glm::vec2 && velocity, glm::vec2 && acceleration, float angularVelocity, float mass)
	: Component(parent), mVelocity(velocity), mAcceleration(acceleration), mAngularVel(angularVelocity), mInverseMass(1 / mass)
{
}



glm::vec2 RigidBody2D::GetVelocity() const
{
	assert(mParent && "Component does not have a parent!!");
	return mVelocity;
}


void RigidBody2D::SetVelocity(glm::vec2 & velocity)
{
	assert(mParent && "Component does not have a parent!!");
	mVelocity = velocity;
}


glm::vec2 RigidBody2D::GetAcceleration() const
{
	assert(mParent && "Component does not have a parent!!");
	return mAcceleration;
}

void RigidBody2D::SetAcceleration(glm::vec2 & acceleration)
{
	assert(mParent && "Component does not have a parent!!");
	mAcceleration = acceleration;
}


float RigidBody2D::GetAngularVelocity() const
{
	assert(mParent && "Component does not have a parent!!");
	return mAngularVel;
}


void RigidBody2D::SetAngularVelocity(float r_velocity)
{
	assert(mParent && "Component does not have a parent!!");
	mAngularVel = r_velocity;
}


float RigidBody2D::GetMass() const
{
	assert(mParent && "Component does not have a parent!!");
	return mInverseMass;
}


void RigidBody2D::SetMass(float mass)
{
	assert(mParent && "Component does not have a parent!!");
	mInverseMass = 1 / mass;
}







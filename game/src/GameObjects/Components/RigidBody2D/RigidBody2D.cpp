/*
FILE: RigidBody2D.cpp
PRIMARY AUTHOR: Sweet

RigidBody Function Bodies

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once


#include "RigidBody2D.h"


RigidBody2D::RigidBody2D() : RigidBody2D(glm::vec2(), glm::vec2(), 0.0f, 1.0f)
{
}


RigidBody2D::RigidBody2D(float angularVelocity, float mass)
	: RigidBody2D(glm::vec2(), glm::vec2(), angularVelocity, mass)
{
}


RigidBody2D::RigidBody2D(glm::vec2 & velocity, glm::vec2 & acceleration)
	: RigidBody2D(velocity, acceleration, 0.0f, 1.0f)
{
}


RigidBody2D::RigidBody2D(glm::vec2 && velocity, glm::vec2 && acceleration)
	: RigidBody2D(velocity, acceleration, 0.0f, 1.0f)
{
}


RigidBody2D::RigidBody2D(glm::vec2 & velocity, glm::vec2 & acceleration, float angularVelocity, float mass)
	: mVelocity(velocity), mAcceleration(acceleration), mAngularVel(angularVelocity), mInverseMass(1 / mass)
{
}


RigidBody2D::RigidBody2D(glm::vec2 && velocity, glm::vec2 && acceleration, float angularVelocity, float mass)
	: mVelocity(velocity), mAcceleration(acceleration), mAngularVel(angularVelocity), mInverseMass(1 / mass)
{
}



glm::vec2 RigidBody2D::GetVelocity() const
{
	return mVelocity;
}


void RigidBody2D::SetVelocity(glm::vec2 & velocity)
{
	mVelocity = velocity;
}


glm::vec2 RigidBody2D::GetAcceleration() const
{
	return mAcceleration;
}

void RigidBody2D::SetAcceleration(glm::vec2 & acceleration)
{
	mAcceleration = acceleration;
}


float RigidBody2D::GetAngularVelocity() const
{
	return mAngularVel;
}


void RigidBody2D::SetAngularVelocity(float r_velocity)
{
	mAngularVel = r_velocity;
}


float RigidBody2D::GetMass() const
{
	return mInverseMass;
}


void RigidBody2D::SetMass(float mass)
{
	mInverseMass = 1 / mass;
}







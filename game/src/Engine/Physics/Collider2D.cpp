/*
FILE: RigidBody.h
PRIMARY AUTHOR: Brett Schiff

Collider2D component and System(Collision Checking)

Copyright © 2017 DigiPen (USA) Corporation.
*/
#include "Collider2D.h"

// getters
glm::vec3 ColliderBox2DComponent::GetDimensions()
{
	return dimensions_;
}

glm::vec3 ColliderBox2DComponent::GetOffset()
{
	return offset_;
}

float ColliderBox2DComponent::GetRotationOffset()
{
	return rotationOffset_;
}

// setters
void ColliderBox2DComponent::SetDimensions(glm::vec3 newDimensions)
{
	dimensions_ = newDimensions;
}

void ColliderBox2DComponent::SetOffset(glm::vec3 newOffset)
{
	offset_ = newOffset;
}

void ColliderBox2DComponent::SetRotationOffset(float newRotationOffset)
{
	rotationOffset_ = newRotationOffset;
}

// methods
	// Passed only one parameter, scales both axes by the same thing
void ColliderBox2DComponent::ScaleDimensions(float xScale, float yScale)
{
	dimensions_.x *= xScale;
	
	if (yScale == 0)
	{
		dimensions_.y *= xScale;
	}
	else
	{
		dimensions_.y *= yScale;
	}
}

void ColliderBox2DComponent::AdjustRotationOffset(float rotationAdjustment)
{
	rotationOffset_ += rotationAdjustment;
}

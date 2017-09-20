/*
FILE: Collider2D.h
PRIMARY AUTHOR: Brett Schiff

Collider2D components and System(Collision Checking)

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "glm/glm.hpp"
#include "../../GameObjectSystem/GameSpace.h"

class Collider2D
{
public:
	// different types(shapes) of colliders
	enum class colliderType
	{
		colliderBox = 0
	};

	// constructor
	Collider2D(colliderType colliderType, glm::vec3 dimensions, glm::vec3 offset = glm::vec3(0), float rotationOffset = 0) : m_colliderType(colliderType), m_dimensions(dimensions), m_offset(offset), m_rotationOffset(rotationOffset)
	{
	}

	// getters
	colliderType GetColliderType();
	glm::vec3 GetDimensions();
	glm::vec3 GetOffset();
	float GetRotationOffset();

	// setters
	void SetColliderType(colliderType colliderType);
	void SetDimensions(glm::vec3 newDimensions);
	void SetOffset(glm::vec3 newOffset);
	void SetRotationOffset(float newRotationOffset);

	// methods
	// Passed only one parameter, scales both axes by the same thing
	void ScaleDimensions(float xScale, float yScale = 0);
	void AdjustRotationOffset(float rotationAdjustment);

private:
	colliderType m_colliderType;
	glm::vec3 m_dimensions;
	glm::vec3 m_offset;
	float m_rotationOffset;
};


class StaticCollider2DComponent
{
public:
	// constructor
	StaticCollider2DComponent(Collider2D::colliderType colliderType, glm::vec3 dimensions, glm::vec3 offset = glm::vec3(0), float rotationOffset = 0) : m_colliderData(colliderType, dimensions, offset, rotationOffset)
	{
	}

	// getter of the Collider
	Collider2D& ColliderData();

private:
	// the actual data of the collider
	Collider2D m_colliderData;
};

class DynamicCollider2DComponent
{
public:
	// constructor
	DynamicCollider2DComponent(Collider2D::colliderType colliderType, glm::vec3 dimensions, glm::vec3 offset = glm::vec3(0), float rotationOffset = 0) : m_colliderData(colliderType, dimensions, offset, rotationOffset)
	{
	}

	// getter of the Collider
	Collider2D& ColliderData();

private:
	// the actual data of the collider
	Collider2D m_colliderData;
};

/*
FILE: Collider2D.h
PRIMARY AUTHOR: Brett Schiff

Collider2D components and System(Collision Checking)

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "glm/glm.hpp"
#include "../../GameObjectSystem/GameSpace.h"
#include "CollisionLayer.h"

constexpr char *collider_types[] =
{
	"Box"
};

class Collider2D
{
public:
	// different types(shapes) of colliders
	enum colliderType
	{
		staticCollider = 1 << 0,  // DO NOT set staticCollider yourself, it is taken care of interally
		colliderBox    = 1 << 1,

		collider_max // Used by the editor
	};

	// constructor
	Collider2D(int colliderType, glm::vec3 dimensions, int collisionLayer = collisionLayers::allCollision, glm::vec3 offset = glm::vec3(0), float rotationOffset = 0) : 
			   m_colliderType(colliderType), m_dimensions(dimensions), m_collisionLayer(collisionLayer), m_offset(offset), m_rotationOffset(rotationOffset)
	{
	}

	// getters
	int GetColliderType();
	glm::vec3 GetDimensions();
	glm::vec3 GetOffset();
	float GetRotationOffset();
	CollisionLayer GetCollisionLayer();

	// setters
	void SetColliderType(colliderType colliderType);
	void SetDimensions(glm::vec3 newDimensions);
	void SetOffset(glm::vec3 newOffset);
	void SetRotationOffset(float newRotationOffset);
	void SetCollisionLayer(CollisionLayer newLayer);

	// methods
	// Passed only one parameter, scales both axes by the same thing
	void ScaleDimensions(float xScale, float yScale = 0);
	void AdjustRotationOffset(float rotationAdjustment);
	bool isStatic();

private:
	friend void ImGui_Collider2D(Collider2D *collider, GameObject *object);
	int m_colliderType;
	glm::vec3 m_dimensions;
	CollisionLayer m_collisionLayer;
	glm::vec3 m_offset;
	float m_rotationOffset;
};


class StaticCollider2DComponent
{
public:
	// constructor
	StaticCollider2DComponent(int colliderType, glm::vec3 dimensions, int collisionLayer = collisionLayers::allCollision, glm::vec3 offset = glm::vec3(0), float rotationOffset = 0) :
							  m_colliderData(colliderType | Collider2D::colliderType::staticCollider, dimensions, collisionLayer, offset, rotationOffset)
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
	DynamicCollider2DComponent(int colliderType, glm::vec3 dimensions, int collisionLayer = collisionLayers::allCollision, glm::vec3 offset = glm::vec3(0), float rotationOffset = 0) :
							   m_colliderData(colliderType, dimensions, collisionLayer, offset, rotationOffset)
	{
	}

	// getter of the Collider
	Collider2D& ColliderData();

private:
	// the actual data of the collider
	Collider2D m_colliderData;
};

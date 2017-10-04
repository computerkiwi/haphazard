/*
FILE: RigidBody.h
PRIMARY AUTHOR: Brett Schiff

Collider2D component and System(Collision Checking)

Copyright © 2017 DigiPen (USA) Corporation.
*/
#include "Collider2D.h"

// getters
int Collider2D::GetColliderType()
{
	return m_colliderType & ~(colliderType::staticCollider);
}

glm::vec3 Collider2D::GetDimensions()
{
	return m_dimensions;
}

glm::vec3 Collider2D::GetOffset()
{
	return m_offset;
}

float Collider2D::GetRotationOffset()
{
	return m_rotationOffset;
}

CollisionLayer Collider2D::GetCollisionLayer()
{
	return m_collisionLayer;
}

// setters
void Collider2D::SetColliderType(colliderType colliderType)
{
	m_colliderType = colliderType;
}

void Collider2D::SetDimensions(glm::vec3 newDimensions)
{
	m_dimensions = newDimensions;
}

void Collider2D::SetOffset(glm::vec3 newOffset)
{
	m_offset = newOffset;
}

void Collider2D::SetRotationOffset(float newRotationOffset)
{
	m_rotationOffset = newRotationOffset;
}

void Collider2D::SetCollisionLayer(CollisionLayer newLayer)
{
	m_collisionLayer = newLayer;
}

// methods
	// Passed only one parameter, scales both axes by the same thing
void Collider2D::ScaleDimensions(float xScale, float yScale)
{
	m_dimensions.x *= xScale;
	
	if (yScale == 0)
	{
		m_dimensions.y *= xScale;
	}
	else
	{
		m_dimensions.y *= yScale;
	}
}

void Collider2D::AdjustRotationOffset(float rotationAdjustment)
{
	m_rotationOffset += rotationAdjustment;
}

bool Collider2D::isStatic()
{
	return m_colliderType & colliderType::staticCollider;
}

Collider2D& StaticCollider2DComponent::ColliderData()
{
	return m_colliderData;
}

Collider2D& DynamicCollider2DComponent::ColliderData()
{
	return m_colliderData;
}

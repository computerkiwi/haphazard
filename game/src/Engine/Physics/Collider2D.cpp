/*
FILE: RigidBody.h
PRIMARY AUTHOR: Brett Schiff

Collider2D component and System(Collision Checking)

Copyright © 2017 DigiPen (USA) Corporation.
*/
#include "Collider2D.h"

// getters
int Collider2D::GetColliderShape() const
{
	return m_colliderShape & ~(colliderType::staticCollider);
}

glm::vec3 Collider2D::GetDimensions() const
{
	return m_dimensions;
}

glm::vec2 Collider2D::GetOffset() const
{
	return m_offset;
}

float Collider2D::GetRotationOffset() const
{
	return m_rotationOffset;
}

CollisionLayer Collider2D::GetCollisionLayer() const
{
	return m_collisionLayer;
}

float Collider2D::GetSelfElasticity() const
{
	return m_selfElasticity;
}

float Collider2D::GetAppliedElasticity() const
{
	return m_appliedElasticity;
}

Collider2D::collisionType Collider2D::GetCollisionType() const
{
	return m_collisionType;
}

int Collider2D::METAGetCollisionType() const
{
	return static_cast<int>(m_collisionType);
}

// setters
void Collider2D::SetColliderShape(colliderType colliderType)
{
	if (m_colliderShape & (colliderType::staticCollider))
	{
		m_colliderShape = colliderType & (colliderType::staticCollider);
	}
	else
	{
		m_colliderShape = colliderType;
	}
}

void Collider2D::SetStatic()
{
	m_colliderShape |= colliderType::staticCollider;
}
void Collider2D::SetDynamic()
{
	m_colliderShape &= ~colliderType::staticCollider;
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

void Collider2D::AddCollidedLayer(collisionLayers layer)
{
	m_layersCollidedWith = m_layersCollidedWith | layer;
}

void Collider2D::RemoveCollidedLayer(collisionLayers layer)
{
	m_layersCollidedWith = m_layersCollidedWith & ~layer;
}

void Collider2D::ClearCollidedLayers()
{
	// set each flag to 0
	m_layersCollidedWith = 0;
}

void Collider2D::SetSelfElasticity(float newElasticity)
{
	m_selfElasticity = newElasticity;
}

void Collider2D::AmplifySelfElasticity(float scalar)
{
	m_selfElasticity *= scalar;
}

void Collider2D::SetAppliedElasticity(float newElasticity)
{
	m_appliedElasticity = newElasticity;
}

void Collider2D::AmplifyAppliedElasticity(float scalar)
{
	m_appliedElasticity *= scalar;
}

void Collider2D::SetCollisionType(Collider2D::collisionType type)
{
	m_collisionType = type;
}

void Collider2D::METASetCollisionType(int type)
{
	m_collisionType = static_cast<Collider2D::collisionType>(type);
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

bool Collider2D::isStatic() const
{
	return m_colliderShape & colliderType::staticCollider;
}

Collider2D& StaticCollider2DComponent::ColliderData()
{
	return m_colliderData;
}

Collider2D& DynamicCollider2DComponent::ColliderData()
{
	return m_colliderData;
}

bool Collider2D::IsCollidingWithLayer(collisionLayers layer) const
{
	return (m_layersCollidedWith & layer);
}

bool Collider2D::ColliderIsShape(colliderType colliderType) const
{
	return (m_colliderShape & ~colliderType::staticCollider) == colliderType;
}


StaticCollider2DComponent::StaticCollider2DComponent(DynamicCollider2DComponent *dyn_collider) : m_colliderData(dyn_collider->ColliderData())
{
	m_colliderData.SetStatic();
}


DynamicCollider2DComponent::DynamicCollider2DComponent(StaticCollider2DComponent *static_collider) : m_colliderData(static_collider->ColliderData())
{
	m_colliderData.SetDynamic();
}

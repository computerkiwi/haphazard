/*
FILE: Collider2D.h
PRIMARY AUTHOR: Brett Schiff

Collider2D components and System(Collision Checking)

Copyright � 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "glm/glm.hpp"
#include "../../GameObjectSystem/GameSpace.h"
#include "CollisionLayer.h"

constexpr char *collider_types[] =
{
	"Box"
};

class DynamicCollider2DComponent;

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

	enum collisionType : int
	{
		solid,
		passthrough,

		collision_types
	};

	// constructor
	Collider2D(glm::vec3 dimensions = glm::vec3(1,1,1), int collisionLayer = collisionLayers::allCollision, int colliderType = Collider2D::colliderType::colliderBox,
		       float selfElasticity = 1, float appliedElasticity = 1, Collider2D::collisionType collisionType = Collider2D::collisionType::solid, glm::vec3 offset = glm::vec3(0), float rotationOffset = 0) :
			   m_colliderShape(colliderType), m_dimensions(dimensions), m_collisionLayer(collisionLayer), m_offset(offset), m_rotationOffset(rotationOffset),
		       m_layersCollidedWith(0), m_selfElasticity(selfElasticity), m_appliedElasticity(appliedElasticity), m_collisionType(collisionType)
	{
	}

	Collider2D(const Collider2D&) = default;

	// getters
	int GetColliderShape();
	glm::vec3 GetDimensions();
	glm::vec3 GetOffset();
	float GetRotationOffset();
	CollisionLayer GetCollisionLayer();
	float GetSelfElasticity();
	float GetAppliedElasticity();
	collisionType GetCollisionType();

	// setters
	void SetColliderShape(colliderType colliderType);
	void SetStatic();
	void SetDynamic();
	void SetDimensions(glm::vec3 newDimensions);
	void SetOffset(glm::vec3 newOffset);
	void SetRotationOffset(float newRotationOffset);
	void SetCollisionLayer(CollisionLayer newLayer);
	void AddCollidedLayer(collisionLayers layer);
	void RemoveCollidedLayer(collisionLayers layer);
	void SetSelfElasticity(float newElasticity);
	void AmplifySelfElasticity(float scalar);
	void SetAppliedElasticity(float newElasticity);
	void AmplifyAppliedElasticity(float scalar);
	void SetCollisionType(collisionType type);

	// methods
	// Passed only one parameter, scales both axes by the same thing
	void ScaleDimensions(float xScale, float yScale = 0);
	void AdjustRotationOffset(float rotationAdjustment);
	bool isStatic();
	bool IsCollidingWithLayer(collisionLayers layer);
	void ClearCollidedLayers();

private:
	friend void ImGui_Collider2D(Collider2D *collider, GameObject object, Editor * editor);
	int m_colliderShape;
	glm::vec3 m_dimensions;
	CollisionLayer m_collisionLayer;
	glm::vec3 m_offset;
	float m_rotationOffset;
	int m_layersCollidedWith;
	float m_selfElasticity;
	float m_appliedElasticity;
	collisionType m_collisionType;

	META_REGISTER(Collider2D)
	{
		// HACK: Do some safety registration.
		META_DefineType(int);
		META_DefineType(float);
		META_DefineType(glm::vec3);
		META_DefineType(CollisionLayer);
		META_DefineType(collisionType);

		META_DefineType(Collider2D);
		META_DefineMember(Collider2D, m_colliderShape, "colliderShape");
		META_DefineMember(Collider2D, m_offset, "offset");
		META_DefineMember(Collider2D, m_dimensions, "dimensions");
		META_DefineMember(Collider2D, m_rotationOffset, "rotationOffset");
		META_DefineMember(Collider2D, m_collisionLayer, "collisionLayer");
		META_DefineMember(Collider2D, m_layersCollidedWith, "layersCollidedWith");
		META_DefineMember(Collider2D, m_selfElasticity, "selfElasticity");
		META_DefineMember(Collider2D, m_appliedElasticity, "appliedElasticity");
		META_DefineMember(Collider2D, m_collisionType, "collisionType");
	}
};


class StaticCollider2DComponent
{
public:
	// constructor
	StaticCollider2DComponent(glm::vec3 dimensions = glm::vec3(1,1,1), int collisionLayer = collisionLayers::allCollision, int colliderType = Collider2D::colliderType::colliderBox,
							  float selfElasticity = 1, float appliedElasticity = 1, Collider2D::collisionType collisionType = Collider2D::collisionType::solid, glm::vec3 offset = glm::vec3(0), float rotationOffset = 0) :
							  m_colliderData(dimensions, collisionLayer, colliderType | Collider2D::colliderType::staticCollider, selfElasticity, appliedElasticity, collisionType, offset, rotationOffset)
	{
	}

	explicit StaticCollider2DComponent(DynamicCollider2DComponent *dyn_collider);

	StaticCollider2DComponent(const StaticCollider2DComponent& other) : m_colliderData(other.m_colliderData)
	{
	}

	// getter of the Collider
	Collider2D& ColliderData();

private:
	// the actual data of the collider
	Collider2D m_colliderData;

	META_REGISTER(StaticCollider2DComponent)
	{
		META_DefineType(StaticCollider2DComponent);
		META_DefineMember(StaticCollider2DComponent, m_colliderData, "colliderData");
	}
};

class DynamicCollider2DComponent
{
public:
	// constructor
	DynamicCollider2DComponent(glm::vec3 dimensions = glm::vec3(1, 1, 1), int collisionLayer = collisionLayers::allCollision, int colliderType = Collider2D::colliderType::colliderBox,
							   float selfElasticity = 1, float appliedElasticity = 1, Collider2D::collisionType collisionType = Collider2D::collisionType::solid, glm::vec3 offset = glm::vec3(0), float rotationOffset = 0) :
							   m_colliderData(dimensions, collisionLayer, colliderType, selfElasticity, appliedElasticity, collisionType, offset, rotationOffset)
	{
	}

	explicit DynamicCollider2DComponent(StaticCollider2DComponent *static_collider);

	DynamicCollider2DComponent(const DynamicCollider2DComponent& other) : m_colliderData(other.m_colliderData)
	{
	}

	// getter of the Collider
	Collider2D& ColliderData();

private:
	// the actual data of the collider
	Collider2D m_colliderData;

	META_REGISTER(DynamicCollider2DComponent)
	{
		META_DefineType(DynamicCollider2DComponent);
		META_DefineMember(DynamicCollider2DComponent, m_colliderData, "colliderData");

	}
};

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

	enum class colliderType
	{
		colliderBox = 0
	};

	Collider2D(colliderType colliderType, glm::vec3 dimensions, glm::vec3 offset, float rotationOffset = 0);

private:
	glm::vec3 dimensions_;
	glm::vec3 offset_;
	float rotationOffset_;
};


class StaticCollider2DComponent
{
public:
	// constructor
	StaticCollider2DComponent(Collider2D::colliderType colliderType, glm::vec3 dimensions, glm::vec3 offset, float rotationOffset = 0) : colliderData_(colliderType, dimensions, offset, rotationOffset)
	{
	}

	Collider2D& GetColliderData();

private:
	Collider2D colliderData_;
};

class DynamicCollider2DComponent
{
public:
	// constructor
	DynamicCollider2DComponent(Collider2D::colliderType colliderType, glm::vec3 dimensions, glm::vec3 offset, float rotationOffset = 0) : colliderData_(colliderType, dimensions, offset, rotationOffset)
	{
	}

	Collider2D& GetColliderData();

private:
	Collider2D colliderData_;
};





class ColliderBox2DComponent
{
public:
	// constructors
	ColliderBox2DComponent(glm::vec3 dimensions, glm::vec3 offset, float rotationOffset = 0) : dimensions_(dimensions), offset_(offset), rotationOffset_(rotationOffset)
	{
	}

	// getters
	glm::vec3 GetDimensions();
	glm::vec3 GetOffset();
	float GetRotationOffset();

	// setters
	void SetDimensions(glm::vec3 newDimensions);
	void SetOffset(glm::vec3 newOffset);
	void SetRotationOffset(float newRotationOffset);

	// methods
		// Passed only one parameter, scales both axes by the same thing
	void ScaleDimensions(float xScale, float yScale = 0);
	void AdjustRotationOffset(float rotationAdjustment);

private:
	glm::vec3 dimensions_;
	glm::vec3 offset_;
	float rotationOffset_;
};

class CollisionSystem : public SystemBase
{
	virtual void Init();

	// Called each frame.
	virtual void Update(float dt);

	// Simply returns the default priority for this system.
	virtual size_t DefaultPriority()
	{
		return 2;
	}
};
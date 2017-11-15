/*
FILE: Raycast.h
PRIMARY AUTHOR: Brett Schiff

Raycasting

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once
#include "glm/glm.hpp"
#include "Collider2D.h"
#include "GameObjectSystem\GameObject.h"
#include "CollisionLayer.h"

// forward declaration
struct MinMax;
class BoxCorners;

class Raycast
{
public:
	// constructor with direction in degrees
	Raycast(ComponentMap<DynamicCollider2DComponent> *allDynamicColliders, ComponentMap<StaticCollider2DComponent> *allStaticColliders, glm::vec2 startPoint, float direction, float range, collisionLayers layer = collisionLayers::allCollision);
	// constructor with direction along a vector
	Raycast(ComponentMap<DynamicCollider2DComponent> *allDynamicColliders, ComponentMap<StaticCollider2DComponent> *allStaticColliders, glm::vec2 startPoint, glm::vec2 direction, float range, collisionLayers layer = collisionLayers::allCollision);

	// getters/setters
	float& Length();
	glm::vec2& Intersection();
	GameObject& GameObjectHit();

private:
	float m_length;
	glm::vec2 m_intersection;
	GameObject m_gameObjectHit;
	collisionLayers m_layer;
};

class BoxCorners
{
public:

	enum corner
	{
		topRight = 0,
		topLeft = 1,
		botLeft = 2,
		botRight = 3
	};

	BoxCorners(const glm::vec2& center, const glm::vec2& dimensions, float rotation);

	// methods
	MinMax ProjectOntoAxis(glm::vec2 axis) const;

	// the corners of the box in this order: topRight, topLeft, botLeft, botRight
	glm::vec2 m_corners[4];
};

float DegreesToRadians(float angleInDegrees);

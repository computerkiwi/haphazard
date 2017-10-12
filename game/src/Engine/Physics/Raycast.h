/*
FILE: Raycast.h
PRIMARY AUTHOR: Brett Schiff

Raycasting

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once
#include "glm/glm.hpp"
#include "Collider2D.h"


class Raycast
{
public:
	// constructor with direction in degrees
	Raycast(ComponentMap<DynamicCollider2DComponent> *allDynamicColliders, ComponentMap<StaticCollider2DComponent> *allStaticColliders, glm::vec2 startPoint, float direction, float range);
	// constructor with direction along a vector
	Raycast(ComponentMap<DynamicCollider2DComponent> *allDynamicColliders, ComponentMap<StaticCollider2DComponent> *allStaticColliders, glm::vec2 startPoint, glm::vec2 direction, float range);

	// getters/setters
	float& Length();
	glm::vec2& Intersection();

private:
	float m_length;
	glm::vec2 m_intersection;
};
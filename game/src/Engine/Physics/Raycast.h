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
	Raycast(ComponentMap<DynamicCollider2DComponent> *allDynamicColliders, ComponentMap<StaticCollider2DComponent> *allStaticColliders, glm::vec3 startPoint, float direction, float range);
	// constructor with direction along a vector
	Raycast(ComponentMap<DynamicCollider2DComponent> *allDynamicColliders, ComponentMap<StaticCollider2DComponent> *allStaticColliders, glm::vec3 startPoint, glm::vec3 direction, float range);

	// getters/setters
	float& Length();
	glm::vec3& Intersection();

private:
	float m_length;
	glm::vec3 m_intersection;
};
/*
FILE: Collider2D.h
PRIMARY AUTHOR: Brett Schiff

Useful tools used throughout the physics system that aren't deserving of their own files individually

Copyright 2017 DigiPen (USA) Corporation.
*/
#pragma once
#include "Raycast.h" // glm::vec2, BoxCorners

class BoxCollider
{
public:
	BoxCollider(const glm::vec2& center, const glm::vec3& dimensions, float rotation);

	friend std::ostream& operator<<(std::ostream& ostream, const BoxCollider& colliderBox);

	glm::vec3 m_topRight;
	glm::vec3 m_botLeft;
	float m_rotation;
};

bool Collision_PointToBoxQuick(const glm::vec2& point, const BoxCorners& box, float boxRotation);

glm::vec3 Collision_AABBToAABB(BoxCollider& Box1, BoxCollider& Box2);
glm::vec3 Collision_AABBToAABB(ComponentHandle<TransformComponent>& AABB1Transform, Collider2D& AABB1Collider, ComponentHandle<TransformComponent>& AABB2Transform, Collider2D& AABB2Collider);

glm::vec3 Collision_SAT(const BoxCorners& Box1, const BoxCorners& Box2);

void DrawSmallBoxAtPosition(glm::vec2 position);

bool Collision_AABBToAABBbool(glm::vec2 botLeft1, glm::vec2 topRight1, glm::vec2 botLeft2, glm::vec2 topRight2);

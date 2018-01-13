/*
FILE: PhysicsInternalTools.h
PRIMARY AUTHOR: Brett Schiff

Useful tools used throughout the physics system that aren't deserving of their own files individually

Copyright 2017 DigiPen (USA) Corporation.
*/
#pragma once
#include <iostream>
#include "glm/glm.hpp"
#include "GameObjectSystem\GameObject.h"

// forward declarations
struct MinMax;
class TransformComponent;

class BoxCorners
{
public:

	// indices into the corner array
	enum corner
	{
		topRight = 0,
		topLeft = 1,
		botLeft = 2,
		botRight = 3
	};

	// constructor
	BoxCorners(const glm::vec2& center, const glm::vec2& dimensions, float rotation);

	// methods
	MinMax ProjectOntoAxis(glm::vec2 axis) const;

	// the corners of the box in this order: topRight, topLeft, botLeft, botRight
	glm::vec2 m_corners[4];
};

class CapsuleInformation
{
public:
	glm::vec2 m_boxCenter;
	glm::vec2 m_boxDimensions;
	glm::vec2 m_topCircleCenter;
	glm::vec2 m_botCircleCenter;
	float m_circleRadius;
	float m_boxRotation;

	CapsuleInformation(glm::vec2 center, glm::vec2 dimensions, float rotation);
};

struct MinMax
{
	float min;
	float max;

	// see if two sets of min/max intersect
	bool Intersects(const MinMax& other);

	// find the overlap between two sets
	float Overlap(const MinMax& other);
};

bool Collision_PointToBoxQuick(const glm::vec2& point, const BoxCorners& box, float boxRotation);

glm::vec3 Collision_AABBToAABB(const BoxCorners& Box1, const BoxCorners& Box2);
glm::vec3 Collision_AABBToAABB(ComponentHandle<TransformComponent>& AABB1Transform, Collider2D& AABB1Collider, ComponentHandle<TransformComponent>& AABB2Transform, Collider2D& AABB2Collider);

glm::vec3 Collision_SAT(const BoxCorners& Box1, const BoxCorners& Box2);

void DrawSmallBoxAtPosition(glm::vec2 position);
void DrawSmallCircleAtPosition(glm::vec2 position);

bool Collision_AABBToAABBbool(glm::vec2 botLeft1, glm::vec2 topRight1, glm::vec2 botLeft2, glm::vec2 topRight2);

// returns the float with the bigger absolute magnitude
float maxAbs(float lhs, float rhs);

float DegreesToRadians(float angleInDegrees);

float RadiansToDegrees(float angleInRadians);

float vectorToDirection(glm::vec2& dirVector);

bool CirclesCollide(glm::vec2 center1, float radius1, glm::vec2 center2, float radius2);

void DrawSmallBoxAtPosition(glm::vec2 position);

void DrawSmallCircleAtPosition(glm::vec2 position);

float CrossP(glm::vec2 vec1, glm::vec2 vec2);

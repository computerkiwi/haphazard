/*
FILE: Raycast.cpp
PRIMARY AUTHOR: Brett Schiff

Raycasting

Copyright © 2017 DigiPen (USA) Corporation.
*/

// WIP

#include <math.h>

#include "Raycast.h"
#include "../../graphics/DebugGraphic.h"
#include "../../GameObjectSystem/GameSpace.h"

#define DEGREES_PER_RADIAN 57.2957795131f

float radiansToDegrees(float angleInRadians)
{
	return angleInRadians * DEGREES_PER_RADIAN;
}

float vectorToDirection(glm::vec3& dirVector)
{
	return atan2(dirVector.x, dirVector.y);
}

class BoxCollider
{
public:
	BoxCollider(const glm::vec3& center, const glm::vec3& dimensions, float rotation);

	glm::vec3 m_topRight;
	glm::vec3 m_topLeft;
	glm::vec3 m_botLeft;
	glm::vec3 m_botRight;
};

BoxCollider::BoxCollider(const glm::vec3& center, const glm::vec3& dimensions, float rotation)
{
	glm::vec3 centerWithVelocity = center;

	// if the box is axis-aligned, the calculation can be done ignoring angles
	if (rotation == 0)
	{
		float xOffset = (.5f * dimensions.x);
		float yOffset = (.5f * dimensions.y);

		m_topRight = centerWithVelocity + (.5f * dimensions);
		m_botLeft = centerWithVelocity - (.5f * dimensions);

		m_topLeft.x = centerWithVelocity.x - xOffset;
		m_topLeft.y = centerWithVelocity.y + yOffset;

		m_botRight.x = centerWithVelocity.x + xOffset;
		m_botRight.y = centerWithVelocity.y - yOffset;
	}
	else
	{
		float xOffsetFromHorizontal = (dimensions.x * 0.5f * cos(rotation));
		float xOffsetFromVertical = (dimensions.y * 0.5f * sin(rotation));

		float yOffsetFromHorizontal = (dimensions.x * 0.5f * sin(rotation));
		float yOffsetFromVertical = (dimensions.y * 0.5f * cos(rotation));

		// calculate the top right corner
		m_topRight.x = centerWithVelocity.x + xOffsetFromHorizontal - xOffsetFromVertical;
		m_topRight.y = centerWithVelocity.y + yOffsetFromHorizontal + yOffsetFromVertical;

		// calculate the top left corner
		m_topRight.x = centerWithVelocity.x - xOffsetFromHorizontal - xOffsetFromVertical;
		m_topRight.y = centerWithVelocity.y - yOffsetFromHorizontal + yOffsetFromVertical;

		// calculate the bottom left corner
		m_botLeft.x = centerWithVelocity.x - xOffsetFromHorizontal + xOffsetFromVertical;
		m_botLeft.y = centerWithVelocity.y - yOffsetFromHorizontal - yOffsetFromVertical;

		// calculate the bottom right corner
		m_topRight.x = centerWithVelocity.x + xOffsetFromHorizontal + xOffsetFromVertical;
		m_topRight.y = centerWithVelocity.y + yOffsetFromHorizontal - yOffsetFromVertical;
	}
}

// class to do the work of raycasting
class RayCastCalculator
{
public:

	RayCastCalculator(ComponentMap<DynamicCollider2DComponent> *allDynamicColliders, ComponentMap<StaticCollider2DComponent> *allStaticColliders, float range, glm::vec3 startPoint, glm::vec3 direction);

	void CalculateCast(BoxCollider& box);

	glm::vec3 m_startPoint;
	glm::vec3 m_direction;
	float m_range;

	float m_length;
	glm::vec3 m_intersection;
};

RayCastCalculator::RayCastCalculator(ComponentMap<DynamicCollider2DComponent> *allDynamicColliders, ComponentMap<StaticCollider2DComponent> *allStaticColliders, float range, glm::vec3 startPoint, glm::vec3 direction) : m_startPoint(startPoint), m_direction(direction), m_range(range), m_length(-1), m_intersection(glm::vec3(0,0,0))
{
	// find a circle around the ray for broad range checking
	float radius = range / 2;
	glm::vec3 circleCenter = startPoint + (radius * direction);
	float radiusSquared = radius * radius;

	// go through all dynamic colliders
	for (auto tDynamicColliderHandle : *allDynamicColliders)
	{
		ComponentHandle<TransformComponent> transform = tDynamicColliderHandle.GetSiblingComponent<TransformComponent>();
		assert(transform.IsValid() && "Transform invalid in debug drawing, see RayCastCalculator in Raycast.cpp");

		// if the collider is a box
		if (tDynamicColliderHandle->ColliderData().GetColliderType() == Collider2D::colliderType::colliderBox)
		{
			BoxCollider corners(transform->Position(), tDynamicColliderHandle->ColliderData().GetDimensions(), transform->GetRotation() + tDynamicColliderHandle->ColliderData().GetRotationOffset());



			CalculateCast(corners);
		}
	}
	// go though all static colliders
	for (auto tStaticColliderHandle : *allStaticColliders)
	{

	}
}

void RayCastCalculator::CalculateCast(BoxCollider& box)
{

}

// constructor with direction in degrees
Raycast::Raycast(ComponentMap<DynamicCollider2DComponent> *allDynamicColliders, ComponentMap<StaticCollider2DComponent> *allStaticColliders, glm::vec3 startPoint, float direction, float range)
{
	glm::vec3 normalizedDirection((float)(cos(direction)), (float)(sin(direction)), 0);

	// calculate the raycast
	RayCastCalculator raycast(allDynamicColliders, allStaticColliders, range, startPoint, normalizedDirection);

	// use the result
	m_length = raycast.m_length;
	m_intersection = raycast.m_intersection;
}

// constructor with direction along a vector
Raycast::Raycast(ComponentMap<DynamicCollider2DComponent> *allDynamicColliders, ComponentMap<StaticCollider2DComponent> *allStaticColliders, glm::vec3 startPoint, glm::vec3 direction, float range)
{
	glm::vec3 normalizedDirection = direction / glm::length(direction);

	// calculate the raycast
	RayCastCalculator raycast(allDynamicColliders, allStaticColliders, range, startPoint, normalizedDirection);

	// use the result
	m_length = raycast.m_length;
	m_intersection = raycast.m_intersection;
}

// getters/setters
float& Raycast::Length()
{
	return m_length;
}

glm::vec3& Raycast::Intersection()
{
	return m_intersection;
}
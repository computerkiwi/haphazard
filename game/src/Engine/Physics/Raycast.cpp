/*
FILE: Raycast.cpp
PRIMARY AUTHOR: Brett Schiff

Raycasting

Copyright © 2017 DigiPen (USA) Corporation.
*/

// WIP

#include <math.h>
#include <algorithm>

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

	// the corners of the box in this order: topRight, topLeft, botLeft, botRight
	glm::vec3 m_corners[4];
};

BoxCollider::BoxCollider(const glm::vec3& center, const glm::vec3& dimensions, float rotation)
{
	glm::vec3 centerWithVelocity = center;

	// if the box is axis-aligned, the calculation can be done ignoring angles
	if (rotation == 0)
	{
		float xOffset = (.5f * dimensions.x);
		float yOffset = (.5f * dimensions.y);

		m_corners[0] = centerWithVelocity + (.5f * dimensions);

		m_corners[1].x = centerWithVelocity.x - xOffset;
		m_corners[1].y = centerWithVelocity.y + yOffset;

		m_corners[2] = centerWithVelocity - (.5f * dimensions);


		m_corners[3].x = centerWithVelocity.x + xOffset;
		m_corners[3].y = centerWithVelocity.y - yOffset;
	}
	else
	{
		float xOffsetFromHorizontal = (dimensions.x * 0.5f * cos(rotation));
		float xOffsetFromVertical = (dimensions.y * 0.5f * sin(rotation));

		float yOffsetFromHorizontal = (dimensions.x * 0.5f * sin(rotation));
		float yOffsetFromVertical = (dimensions.y * 0.5f * cos(rotation));

		// calculate the top right corner
		m_corners[0].x = centerWithVelocity.x + xOffsetFromHorizontal - xOffsetFromVertical;
		m_corners[0].y = centerWithVelocity.y + yOffsetFromHorizontal + yOffsetFromVertical;

		// calculate the top left corner
		m_corners[1].x = centerWithVelocity.x - xOffsetFromHorizontal - xOffsetFromVertical;
		m_corners[1].y = centerWithVelocity.y - yOffsetFromHorizontal + yOffsetFromVertical;

		// calculate the bottom left corner
		m_corners[2].x = centerWithVelocity.x - xOffsetFromHorizontal + xOffsetFromVertical;
		m_corners[2].y = centerWithVelocity.y - yOffsetFromHorizontal - yOffsetFromVertical;

		// calculate the bottom right corner
		m_corners[3].x = centerWithVelocity.x + xOffsetFromHorizontal + xOffsetFromVertical;
		m_corners[3].y = centerWithVelocity.y + yOffsetFromHorizontal - yOffsetFromVertical;
	}
}

bool CirclesCollide(glm::vec3 center1, float radius1, glm::vec3 center2, float radius2)
{
	glm::vec3 distanceVec = center1 - center2;
	float distanceSquared = (distanceVec.x * distanceVec.x) + (distanceVec.y * distanceVec.y);
	
	return distanceSquared < ((radius1 * radius1) + (radius2 * radius2));
}

// class to do the work of raycasting
class RayCastCalculator
{
public:

	RayCastCalculator(ComponentMap<DynamicCollider2DComponent> *allDynamicColliders, ComponentMap<StaticCollider2DComponent> *allStaticColliders, float range, glm::vec3 startPoint, glm::vec3 direction);

	void CalculateCastBox(BoxCollider& box);

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

	// go through all dynamic colliders
	for (auto tDynamicColliderHandle : *allDynamicColliders)
	{
		ComponentHandle<TransformComponent> transform = tDynamicColliderHandle.GetSiblingComponent<TransformComponent>();
		assert(transform.IsValid() && "Transform invalid in debug drawing, see RayCastCalculator in Raycast.cpp");

		// if the collider is a box
		if (tDynamicColliderHandle->ColliderData().GetColliderType() == Collider2D::colliderType::colliderBox)
		{
			glm::vec3 boxCenter = transform->Position() + tDynamicColliderHandle->ColliderData().GetOffset();
			glm::vec3 boxDimenions = tDynamicColliderHandle->ColliderData().GetDimensions();

			BoxCollider corners(boxCenter, boxDimenions, transform->GetRotation() + tDynamicColliderHandle->ColliderData().GetRotationOffset());

			// circle collision to quickly eliminate far away objects
			if (!CirclesCollide(circleCenter, radius, boxCenter, std::max(boxDimenions.x, boxDimenions.y) / 2))
			{
				continue;
			}

			CalculateCastBox(corners);
		}
	}
	// go though all static colliders
	for (auto tStaticColliderHandle : *allStaticColliders)
	{

	}
}

float CrossP(glm::vec3 vec1, glm::vec3 vec2)
{
	return (vec1.x * vec2.y) - (vec1.y * vec2.x);
}

void RayCastCalculator::CalculateCastBox(BoxCollider& box)
{
	// check each side
	for (int i = 0; i < 4; i++)
	{
		// the points where each ray originates
		glm::vec3 a/*boxPoint*/ = box.m_corners[i];
		glm::vec3 c/*castPoint*/ = m_startPoint;

		// the length and direction of each ray
		glm::vec3 r/*boxRay*/ = box.m_corners[(i + 1) % 4] - a;
		glm::vec3 s/*castRay*/ = m_direction * m_range;

		// these are calculations that would be done more than once, they are stored here early for efficiency's sake
		glm::vec3 vecBetweenPoints = c - a;
		float rayCrossP = CrossP(r, s);

		// scalar into the ray at which it intersects the edge
		float u = CrossP(vecBetweenPoints, r) / rayCrossP;
		// scalar into the edge at which the ray intersects it
		float t = CrossP(vecBetweenPoints, s) / rayCrossP;

		// if there was an intersection
		if (u < 0 && u < 1 && t > 0 && t < 1)
		{
			// calculate the intersection point
			glm::vec3 intersection = c + (u * s);
			float intersectLengthSquared = (intersection.x * intersection.x) + (intersection.y * intersection.y);

			// if that intersection was shorter than the currently stored one
			if (intersectLengthSquared < (m_length * m_length) || m_length == -1)
			{
				// set the new intersection
				m_intersection = intersection;
				m_length = sqrt(intersectLengthSquared);
			}
		}
	}
}

// constructor with direction in degrees
Raycast::Raycast(ComponentMap<DynamicCollider2DComponent> *allDynamicColliders, ComponentMap<StaticCollider2DComponent> *allStaticColliders, glm::vec3 startPoint, float direction, float range)
{
	glm::vec3 normalizedDirection((float)(cos(direction)), (float)(sin(direction)), 0);

	// calculate the raycast
	RayCastCalculator raycast(allDynamicColliders, allStaticColliders, range, startPoint, normalizedDirection);

	// use the result if it hit something
	if (raycast.m_length != -1)
	{
		m_length = raycast.m_length;
		m_intersection = raycast.m_intersection;
	}
	else // else set the full ray
	{
		m_length = range;
		m_intersection = startPoint + (normalizedDirection * range);
	}
}

// constructor with direction along a vector
Raycast::Raycast(ComponentMap<DynamicCollider2DComponent> *allDynamicColliders, ComponentMap<StaticCollider2DComponent> *allStaticColliders, glm::vec3 startPoint, glm::vec3 direction, float range)
{
	glm::vec3 normalizedDirection = direction / glm::length(direction);

	// calculate the raycast
	RayCastCalculator raycast(allDynamicColliders, allStaticColliders, range, startPoint, normalizedDirection);

	// use the result if it hit something
	if (raycast.m_length != -1)
	{
		m_length = raycast.m_length;
		m_intersection = raycast.m_intersection;
	}
	else
	{
		m_length = range;
		m_intersection = startPoint + (normalizedDirection * range);
	}
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
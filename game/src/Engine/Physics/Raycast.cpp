/*
FILE: Raycast.cpp
PRIMARY AUTHOR: Brett Schiff

Raycasting

Copyright © 2017 DigiPen (USA) Corporation.
*/

// WIP

#include <cmath>
#include <algorithm>
#include <iostream>

#include "Raycast.h"
#include "../../graphics/DebugGraphic.h"
#include "../../GameObjectSystem/GameSpace.h"
#include "GameObjectSystem\TransformComponent.h"

#define DEGREES_PER_RADIAN 57.2957795131f

void DrawSmallBoxAtPosition(glm::vec2 position)
{
	DebugGraphic::DrawShape(position, glm::vec2(.1f, .1f), 0, glm::vec4(1, 0, 1, 1));
}

// draws a ray, assumes direction to be already normalized
void DrawRayNormalized(glm::vec2 position, glm::vec2 direction, float length)
{
	DebugGraphic::DrawShape(position + (direction * length * .5f), glm::vec2(length, .03f), atan2(direction.y, direction.x), glm::vec4(0, 1, 1, 1));
}

// draws a ray
void DrawRay(glm::vec2 position, glm::vec2 direction)
{
	DebugGraphic::DrawShape(position + (direction * .5f), glm::vec2(glm::length(direction), .05f), atan2(direction.y, direction.x), glm::vec4(1, 1, 0, 1));
}

float DegreesToRadians(float angleInDegrees)
{
	return angleInDegrees / DEGREES_PER_RADIAN;
}

float RadiansToDegrees(float angleInRadians)
{
	return angleInRadians * DEGREES_PER_RADIAN;
}

float vectorToDirection(glm::vec2& dirVector)
{
	return atan2(dirVector.y, dirVector.x);
}

BoxCorners::BoxCorners(const glm::vec2& center, const glm::vec2& dimensions, float rotation)
{
	glm::vec2 boxCenter = center;

	// if the box is axis-aligned, the calculation can be done ignoring angles
	if (rotation == 0)
	{
		float xOffset = (.5f * dimensions.x);
		float yOffset = (.5f * dimensions.y);

		m_corners[0] = boxCenter + (.5f * dimensions);

		m_corners[1].x = boxCenter.x - xOffset;
		m_corners[1].y = boxCenter.y + yOffset;

		m_corners[2] = boxCenter - (.5f * dimensions);


		m_corners[3].x = boxCenter.x + xOffset;
		m_corners[3].y = boxCenter.y - yOffset;
	}
	else
	{
		float cosRotation = cos(DegreesToRadians(rotation));
		float sinRotation = sin(DegreesToRadians(rotation));

		float xOffsetFromHorizontal = (dimensions.x * 0.5f * cosRotation);
		float xOffsetFromVertical = (dimensions.y * 0.5f * sinRotation);

		float yOffsetFromHorizontal = (dimensions.x * 0.5f * sinRotation);
		float yOffsetFromVertical = (dimensions.y * 0.5f * cosRotation);

		// calculate the top right corner
		m_corners[0].x = boxCenter.x + xOffsetFromHorizontal - xOffsetFromVertical;
		m_corners[0].y = boxCenter.y + yOffsetFromHorizontal + yOffsetFromVertical;

		// calculate the top left corner
		m_corners[1].x = boxCenter.x - xOffsetFromHorizontal - xOffsetFromVertical;
		m_corners[1].y = boxCenter.y - yOffsetFromHorizontal + yOffsetFromVertical;

		// calculate the bottom left corner
		m_corners[2].x = boxCenter.x - xOffsetFromHorizontal + xOffsetFromVertical;
		m_corners[2].y = boxCenter.y - yOffsetFromHorizontal - yOffsetFromVertical;

		// calculate the bottom right corner
		m_corners[3].x = boxCenter.x + xOffsetFromHorizontal + xOffsetFromVertical;
		m_corners[3].y = boxCenter.y + yOffsetFromHorizontal - yOffsetFromVertical;
	}
}

bool CirclesCollide(glm::vec2 center1, float radius1, glm::vec2 center2, float radius2)
{
	glm::vec2 distanceVec = center1 - center2;
	float distanceSquared = (distanceVec.x * distanceVec.x) + (distanceVec.y * distanceVec.y);
	
	return distanceSquared < ((radius1 + radius2) * (radius1 + radius2));
}

// class to do the work of raycasting
class RayCastCalculator
{
public:

	RayCastCalculator(ComponentMap<DynamicCollider2DComponent> *allDynamicColliders, ComponentMap<StaticCollider2DComponent> *allStaticColliders, float range, glm::vec2 startPoint, glm::vec2 direction);

	void Raycast(glm::vec2 raycastCenter, float raycastRadius, ComponentHandle<TransformComponent> transform, Collider2D colliderData);

	void CalculateCastBox(BoxCorners& box, GameObject& gameObject);

	glm::vec2 m_startPoint;
	glm::vec2 m_direction;
	float m_range;

	float m_length;
	glm::vec2 m_intersection;

	GameObject m_gameObjectHit;
};

void RayCastCalculator::Raycast(glm::vec2 raycastCenter, float raycastRadius, ComponentHandle<TransformComponent> transform, Collider2D colliderData)
{
	// if the collider is a box
	if (colliderData.GetColliderType() == Collider2D::colliderType::colliderBox)
	{
		glm::vec2 boxCenter = transform->GetPosition();
		glm::vec2 colliderOffset = colliderData.GetOffset();
		float rotation = transform->GetRotation() + colliderData.GetRotationOffset();
		glm::vec2 boxDimenions = colliderData.GetDimensions();

		BoxCorners corners(boxCenter, boxDimenions, transform->GetRotation() + colliderData.GetRotationOffset());

		// if the collider has an offset from the object, take it into account
		if (colliderOffset.x || colliderOffset.y)
		{
			if (rotation == 0)
			{
				// add the offset to the center
				boxCenter = static_cast<glm::vec2>(transform->GetPosition()) + colliderOffset;
			}
			else
			{
				// calculate the center using the rotated offset
				glm::vec2 botLeftCorner = corners.m_corners[BoxCorners::corner::botLeft];
				glm::vec2 topRightCorner = corners.m_corners[BoxCorners::corner::topRight];

				boxCenter = botLeftCorner + (.5f * (topRightCorner - botLeftCorner));
			}

			corners = BoxCorners(boxCenter, boxDimenions, transform->GetRotation() + colliderData.GetRotationOffset());
		}

		// circle collision to quickly eliminate far away objects
		if (CirclesCollide(raycastCenter, raycastRadius, boxCenter, std::max(boxDimenions.x, boxDimenions.y)))
		{
			CalculateCastBox(corners, transform.GetGameObject());
		}
	}
}

RayCastCalculator::RayCastCalculator(ComponentMap<DynamicCollider2DComponent> *allDynamicColliders, ComponentMap<StaticCollider2DComponent> *allStaticColliders, float range, glm::vec2 startPoint, glm::vec2 direction) : m_startPoint(startPoint), m_direction(direction), m_range(range), m_length(-1), m_intersection(glm::vec2(0,0)), m_gameObjectHit(INVALID_GAMEOBJECT_ID)
{
	// find a circle around the ray for broad range checking
	float radius = range / 2.0f;
	glm::vec2 circleCenter = startPoint + (radius * direction);

	// go through all dynamic colliders
	for (auto tDynamicColliderHandle : *allDynamicColliders)
	{
		ComponentHandle<TransformComponent> transform = tDynamicColliderHandle.GetSiblingComponent<TransformComponent>();
		assert(transform.IsValid() && "Transform invalid in debug drawing, see RayCastCalculator in Raycast.cpp");

		Raycast(circleCenter, radius, transform, tDynamicColliderHandle->ColliderData());
	}
	// go though all static colliders
	for (auto tStaticColliderHandle : *allStaticColliders)
	{
		ComponentHandle<TransformComponent> transform = tStaticColliderHandle.GetSiblingComponent<TransformComponent>();
		assert(transform.IsValid() && "Transform invalid in debug drawing, see RayCastCalculator in Raycast.cpp");

		Raycast(circleCenter, radius, transform, tStaticColliderHandle->ColliderData());
	}
}

float CrossP(glm::vec2 vec1, glm::vec2 vec2)
{
	return (vec1.x * vec2.y) - (vec1.y * vec2.x);
}

void RayCastCalculator::CalculateCastBox(BoxCorners& box, GameObject& gameObject)
{
	// check each side
	for (int i = 0; i < 4; i++)
	{
		// the points where each ray originates
		glm::vec2 a/*boxPoint*/ = box.m_corners[i];
		glm::vec2 c/*castPoint*/ = m_startPoint;

		// the length and direction of each ray
		glm::vec2 r/*boxRay*/ = box.m_corners[(i + 1) % 4] - a;
		glm::vec2 s/*castRay*/ = m_direction * m_range;

		// these are calculations that would be done more than once, they are stored here early for efficiency's sake
		glm::vec2 vecBetweenPoints = c - a;
		float rayCrossP = CrossP(r, s);

		// scalar into the ray at which it intersects the edge
		float u = CrossP(vecBetweenPoints, r) / rayCrossP;
		// scalar into the edge at which the ray intersects it
		float t = CrossP(vecBetweenPoints, s) / rayCrossP;

		// if there was an intersection
		if (u > 0 && u < 1 && t > 0 && t < 1)
		{
			// calculate the intersection point
			glm::vec2 intersection = c + (u * s);

			glm::vec2 length = intersection - c;

			float intersectLengthSquared = (length.x * length.x) + (length.y * length.y);

			// if that intersection was shorter than the currently stored one
			if ((intersectLengthSquared < (m_length * m_length)) || m_length == -1)
			{
				// set the new intersection
				m_intersection = intersection;
				m_length = sqrt(intersectLengthSquared);
				m_gameObjectHit = gameObject;
			}
		}
	}
}

// constructor with direction in degrees
Raycast::Raycast(ComponentMap<DynamicCollider2DComponent> *allDynamicColliders, ComponentMap<StaticCollider2DComponent> *allStaticColliders, glm::vec2 startPoint, float direction, float range) : m_gameObjectHit(INVALID_GAMEOBJECT_ID)
{
	glm::vec2 normalizedDirection((float)(cos(direction)), (float)(sin(direction)));

	// calculate the raycast
	RayCastCalculator raycast(allDynamicColliders, allStaticColliders, range, startPoint, normalizedDirection);

	// use the result if it hit something
	if (raycast.m_length != -1)
	{
		m_length = raycast.m_length;
		m_intersection = raycast.m_intersection;
		m_gameObjectHit = raycast.m_gameObjectHit;
	}
	else // else set the full ray
	{
		m_length = range;
		m_intersection = startPoint + (normalizedDirection * range);
	}
}

// constructor with direction along a vector
Raycast::Raycast(ComponentMap<DynamicCollider2DComponent> *allDynamicColliders, ComponentMap<StaticCollider2DComponent> *allStaticColliders, glm::vec2 startPoint, glm::vec2 direction, float range)
{
	glm::vec2 normalizedDirection = direction / glm::length(direction);

	// calculate the raycast
	RayCastCalculator raycast(allDynamicColliders, allStaticColliders, range, startPoint, normalizedDirection);

	// use the result if it hit something
	if (raycast.m_length != -1)
	{
		m_length = raycast.m_length;
		m_intersection = raycast.m_intersection;
		m_gameObjectHit = raycast.m_gameObjectHit;
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

glm::vec2& Raycast::Intersection()
{
	return m_intersection;
}

GameObject& Raycast::GameObjectHit()
{
	return m_gameObjectHit;
}
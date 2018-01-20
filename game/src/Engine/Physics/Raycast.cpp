/*
FILE: Raycast.cpp
PRIMARY AUTHOR: Brett Schiff

Raycasting

Copyright © 2017 DigiPen (USA) Corporation.
*/

#include <cmath>
#include <algorithm>
#include <iostream>
#include <math.h>

#include "Raycast.h"
#include "PhysicsInternalTools.h"
#include "../../graphics/DebugGraphic.h"
#include "../../GameObjectSystem/GameSpace.h"
#include "GameObjectSystem\TransformComponent.h"
#include "Engine\Engine.h"

static bool debugShowRaycasts = false;

void debugSetDisplayRaycasts(bool raycastsShown)
{
	debugShowRaycasts = raycastsShown;
}

bool debugAreRaycastsDisplayed()
{
	return debugShowRaycasts;
}

// draws a ray, assumes direction to be already normalized
void DrawNormalizedRay(glm::vec2 position, glm::vec2 direction, float length)
{
	DebugGraphic::DrawSquare(position + (direction * length * .5f), glm::vec2(length, .03f), atan2(direction.y, direction.x), glm::vec4(0, 1, 1, 1));
}

// draws a ray
void DrawRay(glm::vec2 position, glm::vec2 direction)
{
	DebugGraphic::DrawSquare(position + (direction * .5f), glm::vec2(glm::length(direction), .05f), atan2(direction.y, direction.x), glm::vec4(1, 1, 0, 1));
}

// class to do the work of raycasting
class RayCastCalculator
{
public:

	RayCastCalculator(ComponentMap<DynamicCollider2DComponent> *allDynamicColliders, ComponentMap<StaticCollider2DComponent> *allStaticColliders, float range, glm::vec2 startPoint, glm::vec2 direction, int layer);

	void Raycast(glm::vec2 raycastCenter, float raycastRadius, ComponentHandle<TransformComponent> transform, const Collider2D& colliderData);

	void CalculateCast_Box(BoxCorners& box, const GameObject& gameObject);
	void CalculateCast_Circle(glm::vec2 center, float radius, const GameObject& gameObject);

	glm::vec2 m_startPoint;
	glm::vec2 m_direction;
	float m_range;

	float m_length;
	glm::vec2 m_intersection;

	GameObject m_gameObjectHit;

	int m_layer;
};

void RayCastCalculator::Raycast(glm::vec2 raycastCenter, float raycastRadius, ComponentHandle<TransformComponent> transform, const Collider2D& colliderData)
{
	// if the collider is a box
	if (colliderData.ColliderIsShape(Collider2D::colliderType::colliderBox))
	{
		glm::vec2 boxCenter = transform->GetPosition() + colliderData.GetOffset();
		float rotation = transform->GetRotation() + colliderData.GetRotationOffset();
		glm::vec2 boxDimenions = colliderData.GetDimensions();

		BoxCorners corners(boxCenter, boxDimenions, transform->GetRotation() + colliderData.GetRotationOffset());

		// if circle collision to quickly eliminate far away objects
		float boxRadius = std::max(boxDimenions.x, boxDimenions.y);
		if (CirclesCollide(raycastCenter, raycastRadius, boxCenter, boxRadius))
		{
			// calculate the actual raycast
			CalculateCast_Box(corners, transform.GetGameObject());
		}
	}
	else if (colliderData.ColliderIsShape(Collider2D::colliderType::colliderCircle))
	{
		glm::vec2 circleCenter = transform->GetPosition() + colliderData.GetOffset();
		float circleDiameter = colliderData.GetDimensions().x;

		// if circle collision to quickly eliminate far away objects
		if (CirclesCollide(raycastCenter, raycastRadius, circleCenter, circleDiameter))
		{
			// calculate the actual raycast
			CalculateCast_Circle(circleCenter, circleDiameter / 2, transform.GetGameObject());
		}
	}
	else if (colliderData.ColliderIsShape(Collider2D::colliderType::colliderCapsule))
	{
		glm::vec2 circleCenter = transform->GetPosition() + colliderData.GetOffset();
		float circleDiameter = std::max(colliderData.GetDimensions().x, colliderData.GetDimensions().y);

		CapsuleInformation capsule(circleCenter, colliderData.GetDimensions(), transform->GetRotation() + colliderData.GetRotationOffset());

		// if circle collision to quickly eliminate far away objects
		if (CirclesCollide(raycastCenter, raycastRadius, circleCenter, circleDiameter))
		{
			BoxCorners corners(capsule.m_boxCenter, capsule.m_boxDimensions, capsule.m_boxRotation);

			GameObject gameObject = transform.GetGameObject();

			// calculate the actual raycast
			CalculateCast_Circle(capsule.m_topCircleCenter, capsule.m_circleRadius, gameObject);
			CalculateCast_Box(corners, gameObject);
			CalculateCast_Circle(capsule.m_botCircleCenter, capsule.m_circleRadius, gameObject);
		}
	}
}

RayCastCalculator::RayCastCalculator(ComponentMap<DynamicCollider2DComponent> *allDynamicColliders, ComponentMap<StaticCollider2DComponent> *allStaticColliders, 
	                                 float range, glm::vec2 startPoint, glm::vec2 direction, int layer)
	                               : m_startPoint(startPoint), m_direction(direction), m_range(range), m_length(-1), m_intersection(glm::vec2(0,0)), 
	                                 m_gameObjectHit(INVALID_GAMEOBJECT_ID), m_layer(layer)
{
	// find a circle around the ray for broad range checking
	float radius = range / 2.0f;
	glm::vec2 circleCenter = startPoint + (radius * direction);

	// go through all dynamic colliders
	for (auto tDynamicColliderHandle : *allDynamicColliders)
	{
		// check if the ray is meant to hit the object based on layer
		if (static_cast<int>(tDynamicColliderHandle->ColliderData().GetCollisionLayer()) & layer || layer & collisionLayers::allCollision)
		{
			ComponentHandle<TransformComponent> transform = tDynamicColliderHandle.GetSiblingComponent<TransformComponent>();
			if (!transform.IsActive())
			{
				continue;
			}
			Assert(transform.IsValid() && "Transform invalid in debug drawing, see RayCastCalculator in Raycast.cpp");

			Raycast(circleCenter, radius, transform, tDynamicColliderHandle->ColliderData());
		}
	}
	// go though all static colliders
	for (auto tStaticColliderHandle : *allStaticColliders)
	{
		// check if the ray is meant to hit the object based on layer
		if (static_cast<int>(tStaticColliderHandle->ColliderData().GetCollisionLayer()) & layer || layer & collisionLayers::allCollision)
		{
			ComponentHandle<TransformComponent> transform = tStaticColliderHandle.GetSiblingComponent<TransformComponent>();
			if (!transform.IsActive())
			{
				continue;
			}
			Assert(transform.IsValid() && "Transform invalid in debug drawing, see RayCastCalculator in Raycast.cpp");

			Raycast(circleCenter, radius, transform, tStaticColliderHandle->ColliderData());
		}
	}
}

void RayCastCalculator::CalculateCast_Box(BoxCorners& box, const GameObject& gameObject)
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

		// if the intersection on the edge line was within bounds
		if (u > 0 && u < 1)
		{
			// scalar into the edge at which the ray intersects it
			float t = CrossP(vecBetweenPoints, s) / rayCrossP;

			// if the intersection on the ray line was within bounds
			if(t > 0 && t < 1)
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
}


void RayCastCalculator::CalculateCast_Circle(glm::vec2 center, float radius, const GameObject& gameObject)
{
	// ray data
	glm::vec2 rayStart = m_startPoint;
	glm::vec2 rayDirection = m_direction * m_range;

	// circle data
	glm::vec2 circleCenter = center;
	float circleRadius = radius;

	// vecter between center of circle and begin point of ray
	glm::vec2 circleToRay = rayStart - circleCenter;

	// quadratic formula to solve for t where t is scalar into ray where collision happens
	float a = glm::dot(rayDirection, rayDirection);
	float b = 2.0f * glm::dot(circleToRay, rayDirection);
	float c = glm::dot(circleToRay, circleToRay) - (circleRadius * circleRadius);

	float determinant = (b * b) - (4 * a * c);
	// if the answer is non-real, there was not intersection
	if (determinant < 0)
	{

	}
	else // else there was a collision, need to find out where
	{
		determinant = sqrt(determinant);

		// both answers to the equation, the real t will be the smaller one
		// NOTE: t1 is ALWAYS the smaller of the two, since determinant and a are both positive
		float t1 = (-b - determinant) / (2 * a);
		// t2 is only calculated if it is necessary, since it will usually not be used

		//if the ray starts outside the circle and intersects it
		if (t1 >= 0 && t1 <= 1)
		{
			glm::vec2 intersectedRay = rayDirection * t1;
			float length = glm::length(intersectedRay);

			// if this cast is shorter than previously recorded casts or it's the first cast
			if (length < m_length || m_length == -1)
			{
				m_intersection = rayStart + intersectedRay;
				m_length = length;
				m_gameObjectHit = gameObject;
			}
			return;
		}

		// now we need to check if t2 intersected
		float t2 = (-b + determinant) / (2 * a);
		if (t2 >= 0 && t2 <= 1) // if the ray started off in the circle and passed through it
		{
			glm::vec2 intersectedRay = rayDirection * t2;
			float length = glm::length(intersectedRay);

			// if this cast is shorter than previously recorded casts or it's the first cast
			if (length < m_length || m_length == -1)
			{
				m_intersection = rayStart + intersectedRay;
				m_length = glm::length(intersectedRay);
				m_gameObjectHit = gameObject;
			}
			return;
		}
	}
}


Raycast::Raycast() : m_length(0),
                     m_startPosition(glm::vec2(0)),
	                 m_intersection(glm::vec2(0)),
	                 m_normalizedDirection(glm::vec2(1, 0)),
	                 m_gameObjectHit(0),
	                 m_layer(collisionLayers::noCollision)
{
}

// constructor with direction in degrees
Raycast::Raycast(ComponentMap<DynamicCollider2DComponent> *allDynamicColliders, ComponentMap<StaticCollider2DComponent> *allStaticColliders, 
	             glm::vec2 startPoint, float direction, float range, collisionLayers layer)
	           : m_startPosition(startPoint), m_normalizedDirection((float)(cos(direction)), (float)(sin(direction))),
	             m_gameObjectHit(INVALID_GAMEOBJECT_ID), m_layer(layer)
{
	// calculate the raycast
	RayCastCalculator raycast(allDynamicColliders, allStaticColliders, range, startPoint, m_normalizedDirection, layer);

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
		m_intersection = startPoint + (m_normalizedDirection * range);
	}

	if (debugAreRaycastsDisplayed())
	{
		Draw();
	}
}

// constructor with direction along a vector
Raycast::Raycast(ComponentMap<DynamicCollider2DComponent> *allDynamicColliders, ComponentMap<StaticCollider2DComponent> *allStaticColliders, 
	             glm::vec2 startPoint, glm::vec2 direction, float range, collisionLayers layer)
	           : m_startPosition(startPoint), m_normalizedDirection(glm::normalize(direction)),
	             m_gameObjectHit(INVALID_GAMEOBJECT_ID), m_layer(layer)
{
	// calculate the raycast
	RayCastCalculator raycast(allDynamicColliders, allStaticColliders, range, startPoint, m_normalizedDirection, layer);

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
		m_intersection = startPoint + (m_normalizedDirection * range);
	}

	if (debugAreRaycastsDisplayed())
	{
		Draw();
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

// methods
void Raycast::Draw(glm::vec4 color, bool drawBoxAtStartPoint, bool drawBoxAtEndPoint)
{
	// draw a box at the start position
	if (drawBoxAtStartPoint)
	{
		DrawSmallBoxAtPosition(m_startPosition);
	}

	// draw a box at the end position
	DebugGraphic::DrawSquare(m_startPosition + (m_normalizedDirection * (m_length / 2)), glm::vec2(m_length, .01f), atan2(m_normalizedDirection.y, m_normalizedDirection.x),
		                    glm::vec4(color.x, color.y, color.z, color.w));

	// draw a box at the intersection
	if (drawBoxAtEndPoint)
	{
		DrawSmallBoxAtPosition(m_intersection);
	}
}

Raycast Raycast::RaycastAngle(size_t space, glm::vec2 startPoint, float direction, float range, int layer)
{
	ComponentMap<DynamicCollider2DComponent>* dynamicColliders = engine->GetSpace(space)->GetComponentMap<DynamicCollider2DComponent>();
	ComponentMap<StaticCollider2DComponent>* staticColliders = engine->GetSpace(space)->GetComponentMap<StaticCollider2DComponent>();

	return Raycast(dynamicColliders, staticColliders, startPoint, direction, range, static_cast<collisionLayers>(layer));
}

Raycast Raycast::RaycastVector(size_t space, glm::vec2 startPoint, glm::vec2 direction, float range, int layer)
{
	ComponentMap<DynamicCollider2DComponent>* dynamicColliders = engine->GetSpace(space)->GetComponentMap<DynamicCollider2DComponent>();
	ComponentMap<StaticCollider2DComponent>* staticColliders = engine->GetSpace(space)->GetComponentMap<StaticCollider2DComponent>();

	return Raycast(dynamicColliders, staticColliders, startPoint, direction, range, static_cast<collisionLayers>(layer));
}

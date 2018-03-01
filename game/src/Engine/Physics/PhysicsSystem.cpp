/*
FILE: PhysicsSystem.cpp
PRIMARY AUTHOR: Brett Schiff

Handles Movement, Colliders, Collision Detection, and Collision Resolution

Copyright � 2017 DigiPen (USA) Corporation.
*/
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <algorithm>

#include "PhysicsSystem.h"
#include "RigidBody.h"
#include "Collider2D.h"
#include "../../graphics/DebugGraphic.h"
#include "GameObjectSystem\GameObject.h"
#include "PhysicsInternalTools.h"
#include "PhysicsUtils.h"
#include "Scripting/ScriptComponent.h"

//TEMP
#include "Raycast.h"

#define MIN x
#define MAX y

bool debugShowHitboxes = false;

void debugSetDisplayHitboxes(bool hitboxesShown)
{
	debugShowHitboxes = hitboxesShown;
}

bool debugAreHitBoxesDisplayed()
{
	return debugShowHitboxes;
}

struct AABBAroundRotatedRectangle
{
	AABBAroundRotatedRectangle(const BoxCorners& object) : 
	                           minX(object.m_corners[0].x), maxX(object.m_corners[0].x), minY(object.m_corners[0].y), maxY(object.m_corners[0].x)
	{
		// go through the other corners
		for (int i = 1; i < 4; ++i)
		{
			float xValue = object.m_corners[i].x;
			float yValue = object.m_corners[i].y;

			// keep track of lowest and highest values
			if (xValue > maxX)
			{
				maxX = xValue;
			}
			else if (xValue < minX)
			{
				minX = xValue;
			}

			if (yValue < minY)
			{
				minY = yValue;
			}
			else if (yValue > maxY)
			{
				maxY = yValue;
			}
		}
	}

	// default constructor
	AABBAroundRotatedRectangle() : minX(0), maxX(0), minY(0), maxY(0)
	{
	}

	float minX;
	float maxX;
	float minY;
	float maxY;
};

std::ostream& operator<<(std::ostream& ostream, const BoxCorners& colliderBox)
{
	ostream << "Top Right Corner: (" << colliderBox.m_corners[BoxCorners::topRight].x << ", " << colliderBox.m_corners[BoxCorners::topRight].y << ")" << std::endl;
	ostream << "Bot Left  Corner: (" << colliderBox.m_corners[BoxCorners::botLeft].x << ", " << colliderBox.m_corners[BoxCorners::botLeft].y << ")" << std::endl;


	return ostream;
}

glm::vec3 Collision_SAT(const BoxCorners& Box1, const BoxCorners& Box2)
{
	const int num_projections = 4;
	float smallestOverlap = -1;
	glm::vec2 smallestAxis;

	// do an optimized AABB check to first rule out distant collisions
	AABBAroundRotatedRectangle AABB1(Box1);
	AABBAroundRotatedRectangle AABB2(Box2);

	bool AABBsAreColliding = Collision_AABBToAABBbool(glm::vec2(AABB1.minX, AABB1.minY), glm::vec2(AABB1.maxX, AABB1.maxY),
		                                              glm::vec2(AABB2.minX, AABB2.minY), glm::vec2(AABB2.maxX, AABB2.maxY));

	//!?!? fix this if bounding boxes are not colliding, objects are not colliding
	/*if (!AABBsAreColliding)
	{
		return glm::vec3(0, 0, 0);
	}*/

	// the vectors onto which each shape will be projected
	glm::vec2 edgeNormals[num_projections] = { { 0,0 } };

	// since these are boxes, we only have to check two of the four side since the rest are parallel, and they are already perpindicular to each other
	edgeNormals[0] = Box1.m_corners[BoxCorners::topRight] - Box1.m_corners[BoxCorners::topLeft];
	edgeNormals[1] = Box1.m_corners[BoxCorners::topRight] - Box1.m_corners[BoxCorners::botRight];
	edgeNormals[2] = Box2.m_corners[BoxCorners::topRight] - Box2.m_corners[BoxCorners::topLeft];
	edgeNormals[3] = Box2.m_corners[BoxCorners::topRight] - Box2.m_corners[BoxCorners::botRight];

	for (int i = 0; i < num_projections; ++i)
	{
		edgeNormals[i] = glm::normalize(edgeNormals[i]);
	}

	// project shapes onto the vectors
	for (int i = 0; i < num_projections; ++i)
	{
		// the axis onto which we will project
		glm::vec2 axis = edgeNormals[i];

		// project corners onto the axis
		MinMax proj1 = Box1.ProjectOntoAxis(axis);
		MinMax proj2 = Box2.ProjectOntoAxis(axis);

		// if the projections are not intersecting, the shapes are not intersecting
		if (!proj1.Intersects(proj2))
		{
			return glm::vec3(0, 0, 0);
		}
		else // else we need to check the overlap for being the smallest escape vector
		{
			float overlap = proj1.Overlap(proj2);

			// if this overlap is less than the last recorded one
			if (abs(overlap) < abs(smallestOverlap) || smallestOverlap == -1)
			{
				smallestOverlap = overlap;
				smallestAxis = axis;
			}
		}
	}

	// if we get here it is guarunteed that there was a collision and all sides have been tested for the shortest overlap
	glm::vec3 escapeVector(smallestAxis * smallestOverlap, 0);

	return -escapeVector;
}

glm::vec3 Collision_SAT(glm::vec2 position1, float rotation1, Collider2D& collider1, glm::vec2 position2, float rotation2, Collider2D& collider2)
{
	glm::vec2 obj1Center = position1 + static_cast<glm::vec2>(collider1.GetOffset());
	glm::vec2 obj1Dimensions = collider1.GetDimensions();
	float obj1Rotation = rotation1 + collider1.GetRotationOffset();

	glm::vec2 obj2Center = position2 + static_cast<glm::vec2>(collider2.GetOffset());
	glm::vec2 obj2Dimensions = collider2.GetDimensions();
	float obj2Rotation = rotation2 + collider2.GetRotationOffset();

	// get the corners of each of the objects
	BoxCorners Box1(obj1Center, obj1Dimensions, obj1Rotation);
	BoxCorners Box2(obj2Center, obj2Dimensions, obj2Rotation);

	return Collision_SAT(Box1, Box2);
}

glm::vec3 Collision_SAT(ComponentHandle<TransformComponent>& transform1, Collider2D& collider1, ComponentHandle<TransformComponent>& transform2, Collider2D& collider2)
{
	return Collision_SAT(transform1->GetPosition(), transform1->GetRotation(), collider1, transform2->GetPosition(), transform2->GetRotation(), collider2);
}

// collision where the circle is the first object
glm::vec3 Collision_SAT_CircleBox(glm::vec2 center1, float radius1, const BoxCorners& rectangle)
{
	const int num_projections = 3;
	float smallestOverlap = -1;
	glm::vec2 smallestAxis;

	// find the closest vertex on the rectangle to the box
	float shortestLengthSquared = HUGE_VALF;
	int index = 5;

	for (int i = 0; i < 4; i++)
	{
		float Xs = center1.x - rectangle.m_corners[i].x;
		float Ys = center1.y - rectangle.m_corners[i].y;
		float lengthSquared = (Xs * Xs) + (Ys * Ys);

		// this vertex is closer
		if (lengthSquared < shortestLengthSquared)
		{
			shortestLengthSquared = lengthSquared;
			index = i;
		}
	}
	Assert(index != 5 && "if this happens, invalid data has been provided, often NAN");

	// get the axis
	glm::vec2 Closestaxis = center1 - rectangle.m_corners[index];

	// the axes onto which shapes will be projected
	glm::vec2 edgeNormals[num_projections] = { {0,0} };
	edgeNormals[0] = Closestaxis;
	edgeNormals[1] = rectangle.m_corners[BoxCorners::topRight] - rectangle.m_corners[BoxCorners::topLeft];
	edgeNormals[2] = rectangle.m_corners[BoxCorners::topRight] - rectangle.m_corners[BoxCorners::botRight];

	// project onto each axis
	for (int i = 0; i < num_projections; ++i)
	{
		// normalize the axis
		edgeNormals[i] = glm::normalize(edgeNormals[i]);

		// this axis of projection
		glm::vec2 axis = edgeNormals[i];

		// project the box onto the axis
		MinMax rectangleProj = rectangle.ProjectOntoAxis(axis);

		// get the two points on the circle that need to be projected
		glm::vec2 circleLowPoint = center1 - (radius1 * axis);
		glm::vec2 circleHighPoint = center1 + (radius1 * axis);

		// project the circle onto the axis
		MinMax circleProj;
		circleProj.min = glm::dot(axis, circleLowPoint);
		circleProj.max = glm::dot(axis, circleHighPoint);

		// if the projections do not intersect
		if (!circleProj.Intersects(rectangleProj))
		{
			return glm::vec3(0, 0, 0);
		}
		else // else we need to check the overlap for being the smallest escape vector
		{
			float overlap = circleProj.Overlap(rectangleProj);

			// if this overlap is less than the last recorded one
			if (abs(overlap) < abs(smallestOverlap) || smallestOverlap == -1)
			{
				smallestOverlap = overlap;
				smallestAxis = axis;
			}
		}
	}

	// if 0 hasn't been returned, all axes detected collision
	glm::vec3 escapeVector(smallestAxis * -smallestOverlap, 0);

	return escapeVector;
}

// collision where the box is the first object
glm::vec3 Collision_SAT_BoxCircle(const BoxCorners& rectangle, glm::vec2 center2, float radius2)
{
	// if the second objects is the circle, the resolution vector will be opposite the first
	glm::vec3 inverseEscapeVector = Collision_SAT_CircleBox(center2, radius2, rectangle);

	return -inverseEscapeVector;
}

glm::vec3 Collision_CircleCircle(glm::vec2 center1, float radius1, glm::vec2 center2, float radius2)
{
	// temporary values to avoid multiple calculations
	float xDifference = center2.x - center1.x;
	float yDifference = center2.y - center1.y;
	float radiusDistance = radius1 + radius2;

	// distance between the centers squared
	float squaredCenterDistance = (xDifference * xDifference) + (yDifference * yDifference);

	// the circles are colliding
	if (radiusDistance * radiusDistance > squaredCenterDistance)
	{
		// get the direction of the resolution
		glm::vec2 direction(center1 - center2);
		direction = glm::normalize(direction);

		// get the magnitude of the resolution
		float magnitude = radiusDistance - sqrtf(squaredCenterDistance);

		direction *= magnitude;

		return glm::vec3(direction, 0);
	}
	else
	{
		return glm::vec3(0);
	}
}

glm::vec3 Collision_SAT_CapsuleBox(glm::vec2 capsuleCenter, glm::vec2 capsuleDimensions, float rotation, const BoxCorners& box)
{
	// information about the capsule
	CapsuleInformation capsule(capsuleCenter, capsuleDimensions, rotation);

	// get the corners of the rectangle of the capsule box
	BoxCorners capsuleBox(capsuleCenter, capsule.m_boxDimensions, rotation);

	// do collision on each of the three components of the capsule
	glm::vec3 escapeVector1 = Collision_SAT_CircleBox(capsule.m_topCircleCenter, capsule.m_circleRadius, box);
	//!?!? MAKE AN OPTIMIZED SAT WHERE ONLY THE LEFT AND RIGHT SIDES ARE CHECKED BECAUSE TOP AND BOTTOM ARE IN CIRCLES
	glm::vec3 escapeVector2 = Collision_SAT(capsuleBox, box);
	glm::vec3 escapeVector3 = Collision_SAT_CircleBox(capsule.m_botCircleCenter, capsule.m_circleRadius, box);

	// the real escape vector to be contructed from the three calculated above
	glm::vec3 realEscapeVector(escapeVector1);

	// use all of the escape vectors to estimate the actual collision that needs to be resolved
	if (escapeVector2.x)
	{
		realEscapeVector.x = maxAbs(realEscapeVector.x, escapeVector2.x);
	}
	if (escapeVector2.y)
	{
		realEscapeVector.y = maxAbs(realEscapeVector.y, escapeVector2.y);
	}
	if (escapeVector3.x)
	{
		realEscapeVector.x = maxAbs(realEscapeVector.x, escapeVector3.x);
	}
	if (escapeVector3.y)
	{
		realEscapeVector.y = maxAbs(realEscapeVector.y, escapeVector3.y);
	}

	return realEscapeVector;
}

glm::vec3 Collision_SAT_BoxCapsule(const BoxCorners& box, glm::vec2 capsuleCenter, glm::vec2 capsuleDimensions, float rotation)
{
	glm::vec3 inverseEscapeVector = Collision_SAT_CapsuleBox(capsuleCenter, capsuleDimensions, rotation, box);

	return -inverseEscapeVector;
}

glm::vec3 Collision_SAT_CapsuleCircle(glm::vec2 capsuleCenter, glm::vec2 capsuleDimensions, float capsuleRotation, glm::vec2 circleCenter, float circleRadius)
{
	// information about the capsule
	CapsuleInformation capsule(capsuleCenter, capsuleDimensions, capsuleRotation);

	// get the corners of the rectangle of the capsule box
	BoxCorners capsuleBox(capsuleCenter, capsule.m_boxDimensions, capsuleRotation);

	// do collision on each of the three components of the capsule
	glm::vec3 escapeVector1 = Collision_CircleCircle(capsule.m_topCircleCenter, capsule.m_circleRadius, circleCenter, circleRadius);
	glm::vec3 escapeVector2 = Collision_SAT_BoxCircle(capsuleBox, circleCenter, circleRadius);
	glm::vec3 escapeVector3 = Collision_CircleCircle(capsule.m_botCircleCenter, capsule.m_circleRadius, circleCenter, circleRadius);

	// the real escape vector to be contructed from the three calculated above
	glm::vec3 realEscapeVector(escapeVector1);

	// use all of the escape vectors to estimate the actual collision that needs to be resolved
	if (escapeVector2.x)
	{
		realEscapeVector.x = maxAbs(realEscapeVector.x, escapeVector2.x);
	}
	if (escapeVector2.y)
	{
		realEscapeVector.y = maxAbs(realEscapeVector.y, escapeVector2.y);
	}
	if (escapeVector3.x)
	{
		realEscapeVector.x = maxAbs(realEscapeVector.x, escapeVector3.x);
	}
	if (escapeVector3.y)
	{
		realEscapeVector.y = maxAbs(realEscapeVector.y, escapeVector3.y);
	}

	return realEscapeVector;
}

glm::vec3 Collision_SAT_CircleCapsule(glm::vec2 circleCenter, float circleRadius, glm::vec2 capsuleCenter, glm::vec2 capsuleDimensions, float capsuleRotation)
{
	glm::vec3 inverseEscapeVector = Collision_SAT_CapsuleCircle(capsuleCenter, capsuleDimensions, capsuleRotation, circleCenter, circleRadius);

	return -inverseEscapeVector;
}

glm::vec3 Collision_SAT_CapsuleCapsule(glm::vec2 capsule1Center, glm::vec2 capsule1Dimensions, float capsule1Rotation, 
	                                   glm::vec2 capsule2Center, glm::vec2 capsule2Dimensions, float capsule2Rotation)
{
	// get information about the other capsule
	CapsuleInformation capsule2(capsule2Center, capsule2Dimensions, capsule2Rotation);

	BoxCorners capsule2Box(capsule2Center, capsule2.m_boxDimensions, capsule2Rotation);

	// do collision on each of the three components of the capsule against the other
	glm::vec3 escapeVector1 = Collision_SAT_CapsuleCircle(capsule1Center, capsule1Dimensions, capsule1Rotation, capsule2.m_topCircleCenter, capsule2.m_circleRadius);
	glm::vec3 escapeVector2 = Collision_SAT_CapsuleBox(capsule1Center, capsule1Dimensions, capsule1Rotation, capsule2Box);
	glm::vec3 escapeVector3 = Collision_SAT_CapsuleCircle(capsule1Center, capsule1Dimensions, capsule1Rotation, capsule2.m_botCircleCenter, capsule2.m_circleRadius);

	// the real escape vector to be contructed from the three calculated above
	glm::vec3 realEscapeVector(escapeVector1);

	// use all of the escape vectors to estimate the actual collision that needs to be resolved
	if (escapeVector2.x)
	{
		realEscapeVector.x = maxAbs(realEscapeVector.x, escapeVector2.x);
	}
	if (escapeVector2.y)
	{
		realEscapeVector.y = maxAbs(realEscapeVector.y, escapeVector2.y);
	}
	if (escapeVector3.x)
	{
		realEscapeVector.x = maxAbs(realEscapeVector.x, escapeVector3.x);
	}
	if (escapeVector3.y)
	{
		realEscapeVector.y = maxAbs(realEscapeVector.y, escapeVector3.y);
	}

	return realEscapeVector;
}

glm::vec3 Collision_AABBToAABB(const BoxCorners& Box1, const BoxCorners& Box2)
{
	glm::vec3 penetrationVector(0);
	glm::vec3 minValue(0);

	if (Box1.m_corners[BoxCorners::topRight].x <= Box2.m_corners[BoxCorners::botLeft].x)
	{
		return glm::vec3(0);
	}
	else
	{
		minValue.x = Box1.m_corners[BoxCorners::topRight].x - Box2.m_corners[BoxCorners::botLeft].x;
	}
	if (Box1.m_corners[BoxCorners::topRight].y <= Box2.m_corners[BoxCorners::botLeft].y)
	{
		return glm::vec3(0);
	}
	else
	{
		minValue.y = Box1.m_corners[BoxCorners::topRight].y - Box2.m_corners[BoxCorners::botLeft].y;
	}
	if (Box1.m_corners[BoxCorners::botLeft].x >= Box2.m_corners[BoxCorners::topRight].x)
	{
		return glm::vec3(0);
	}
	else
	{
		if (abs(Box1.m_corners[BoxCorners::botLeft].x - Box2.m_corners[BoxCorners::topRight].x) < abs(minValue.x))
		{
			minValue.x = Box1.m_corners[BoxCorners::botLeft].x - Box2.m_corners[BoxCorners::topRight].x;
		}
	}
	if (Box1.m_corners[BoxCorners::botLeft].y >= Box2.m_corners[BoxCorners::topRight].y)
	{
		return glm::vec3(0);
	}
	else
	{
		if (abs(Box1.m_corners[BoxCorners::botLeft].y - Box2.m_corners[BoxCorners::topRight].y) < abs(minValue.y))
		{
			minValue.y = Box1.m_corners[BoxCorners::botLeft].y - Box2.m_corners[BoxCorners::topRight].y;
		}
	}

	// if the resolution needs to be primarily along the x axis
	if (abs(minValue.x) < abs(minValue.y))
	{
		penetrationVector.x = minValue.x;
	}
	else // if the resolution needs to be primarily along the y axis
	{
		penetrationVector.y = minValue.y;
	}

	return -penetrationVector;
}

glm::vec3 Collision_AABBToAABB(ComponentHandle<TransformComponent>& AABB1Transform, Collider2D& AABB1Collider, ComponentHandle<TransformComponent>& AABB2Transform, Collider2D& AABB2Collider)
{
	BoxCorners Box1(AABB1Transform->GetPosition() + AABB1Collider.GetOffset(), AABB1Collider.GetDimensions(), AABB1Transform->GetRotation() + AABB1Collider.GetRotationOffset());
	BoxCorners Box2(AABB2Transform->GetPosition() + AABB2Collider.GetOffset(), AABB2Collider.GetDimensions(), AABB2Transform->GetRotation() + AABB2Collider.GetRotationOffset());

	return Collision_AABBToAABB(Box1, Box2);
}

bool Collision_AABBToAABBbool(glm::vec2 botLeft1, glm::vec2 topRight1, glm::vec2 botLeft2, glm::vec2 topRight2)
{
	// if object1 is above object2
	if (botLeft1.y >= topRight2.y)
	{
		return false;
	}
	// if object1 is left of object2
	if (topRight1.x <= botLeft2.x)
	{
		return false;
	}
	// if object1 is right of object2
	if (botLeft1.x >= topRight2.x)
	{
		return false;
	}
	// if the point is below the object, return false
	if (topRight1.y <= botLeft2.y)
	{
		return false;
	}

	return true;
}

bool Collision_PointToBoxQuick(const glm::vec2& point, const BoxCorners& box, float boxRotation)
{
	// use AABB collision if the box is not rotated
	if (boxRotation == 0)
	{
		// get the top right and bottom left corners of the object
		const glm::vec2 botLeftCorner = box.m_corners[BoxCorners::botLeft];
		const glm::vec2 topRightCorner = box.m_corners[BoxCorners::topRight];

		// if the point is above the object, return false
		if (point.y >= topRightCorner.y)
		{
			return false;
		}
		// if the point is let of the object, return false
		if (point.x <= botLeftCorner.x)
		{
			return false;
		}
		// if the point is right of the object, return false
		if (point.x >= topRightCorner.x)
		{
			return false;
		}
		// if the point is below the object, return false
		if (point.y <= botLeftCorner.y)
		{
			return false;
		}
	}
	else
	{
		glm::vec3 result = Collision_SAT(BoxCorners(point, glm::vec2(.000001f, .000001f), 0), box);

		if (result.x || result.y)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return true;
}

void printAMatrix(glm::mat3 matrix)
{
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			std::cout << matrix[i][j] << "   ";
		}
		std::cout << "\n";
	}

}

void ResolveDynDynCollision(float dt, glm::vec3* collisionData, ComponentHandle<DynamicCollider2DComponent> collider1, ComponentHandle<TransformComponent> transform1, ComponentHandle<DynamicCollider2DComponent> collider2, ComponentHandle<TransformComponent> transform2)
{
	if (collider2->ColliderData().GetCollisionType() == Collider2D::collisionType::solid && collider1->ColliderData().GetCollisionType() == Collider2D::collisionType::solid)
	{
		ComponentHandle<RigidBodyComponent> rigidBody1 = collider1.GetSiblingComponent<RigidBodyComponent>();
		ComponentHandle<RigidBodyComponent> rigidBody2 = collider2.GetSiblingComponent<RigidBodyComponent>();
		// make sure rigidbodies were successfully retrieved
		Assert(rigidBody1.IsValid() && rigidBody2.IsValid() && "Rigidbody(s) invalid. See ResolveDynDynCollision in PhysicsSystem.cpp\n");

		glm::vec3 resolutionVector = *collisionData;
		glm::vec3 dividingVector((*collisionData).y, -(*collisionData).x, 0);

		//std::cout << dividingVector.x << " " << dividingVector.y << "\n";

		float a = dividingVector.x;
		float b = dividingVector.y;
		float a2 = dividingVector.x * dividingVector.x;
		float b2 = dividingVector.y * dividingVector.y;

		// get the matrix for reflecting over dividing vector
		glm::mat3 refMtrx(1 - ((2 * b2) / (a2 + b2)), (2 * a*b) / (a2 + b2), 0,
			((2 * a*b) / (a2 + b2)), ((2 * b2) / (a2 + b2)) - 1, 0,
			0, 0, 1
		);

		// check to see if the objects are moving in the same direction
		float xCompare = rigidBody1->Velocity().x / rigidBody2->Velocity().x;
		float yCompare = rigidBody1->Velocity().y / rigidBody2->Velocity().y;

		//!?!? quick cheap solution - replace with a real one later
		if (collider1->ColliderData().GetRotationOffset() + collider2->ColliderData().GetRotationOffset() + transform1->GetRotation() + transform2->GetRotation() == 0)
		{
			glm::vec2 pos1 = transform1->GetPosition();
			glm::vec2 pos2 = transform2->GetPosition();

			glm::vec2 halfresolve = resolutionVector * 0.5f;

			transform1->SetPosition(pos1 + halfresolve);
			transform2->SetPosition(pos2 - halfresolve);
		}

		// calculate the elasticity multiplier on each object
		float obj1ElasticityMuliplier = collider1->ColliderData().GetSelfElasticity() * collider2->ColliderData().GetAppliedElasticity();
		float obj2ElasticityMuliplier = collider2->ColliderData().GetSelfElasticity() * collider1->ColliderData().GetAppliedElasticity();

		// if they are not going in the same direction
		if (xCompare < 0 || yCompare < 0)
		{
			rigidBody1->SetVelocity((refMtrx * rigidBody1->Velocity()) * obj1ElasticityMuliplier);
			rigidBody2->SetVelocity((refMtrx * rigidBody2->Velocity()) * obj2ElasticityMuliplier);
		}
		else // if they are going in the same direction
		{
			// the magnitude of each object's velocity
			float obj1SquaredMagnitude = (rigidBody1->Velocity().x * rigidBody1->Velocity().x) + (rigidBody1->Velocity().y * rigidBody1->Velocity().y);
			float obj2SquaredMagnitude = (rigidBody2->Velocity().x * rigidBody2->Velocity().x) + (rigidBody2->Velocity().y * rigidBody2->Velocity().y);

			if (obj1SquaredMagnitude < obj2SquaredMagnitude)
			{
				rigidBody1->SetVelocity(rigidBody1->Velocity());
				rigidBody2->SetVelocity((refMtrx * rigidBody2->Velocity()) * obj2ElasticityMuliplier);
			}
			else
			{
				rigidBody1->SetVelocity((refMtrx * rigidBody1->Velocity()) * obj1ElasticityMuliplier);
				rigidBody2->SetVelocity(rigidBody2->Velocity());
			}
		}
	}
}

void ResolveDynStcCollision(float dt, glm::vec3* collisionData, ComponentHandle<DynamicCollider2DComponent> collider1, ComponentHandle<StaticCollider2DComponent> collider2)
{
	if (collider2->ColliderData().GetCollisionType() == Collider2D::collisionType::oneWay)
	{
		ComponentHandle<RigidBodyComponent> rigidBody1 = collider1.GetSiblingComponent<RigidBodyComponent>();
		// make sure rigidbodies were successfully retrieved
		Assert(rigidBody1.IsValid() && "Rigidbody invalid. See ResolveDynStcCollision in PhysicsSystem.cpp\n");

		ComponentHandle<TransformComponent> transform1 = collider1.GetSiblingComponent<TransformComponent>();
		Assert(rigidBody1.IsValid() && "Transform is invalid. See REsolveDynStcCollision in PhysicsSystem.cpp\n");

		// the bottom of the object before it was moved by velocity
		float objBot = transform1->GetPosition().y - (collider1->ColliderData().GetDimensions().y / 2) - (rigidBody1->Velocity().y * dt);

		ComponentHandle<TransformComponent> transform2 = collider2.GetSiblingComponent<TransformComponent>();
		Assert(transform2.IsValid() && "Transform is invalid. See REsolveDynStcCollision in PhysicsSystem.cpp\n");

		float platformTop = transform2->GetPosition().y + collider2->ColliderData().GetDimensions().y / 2;

		if (objBot <= platformTop)
		{
			return;
		}
	}

	if (collider2->ColliderData().GetCollisionType() != Collider2D::collisionType::passthrough && collider1->ColliderData().GetCollisionType() == Collider2D::collisionType::solid)
	{
		ComponentHandle<RigidBodyComponent> rigidBody1 = collider1.GetSiblingComponent<RigidBodyComponent>();
		// make sure rigidbodies were successfully retrieved
		Assert(rigidBody1.IsValid() && "Rigidbody invalid. See ResolveDynStcCollision in PhysicsSystem.cpp\n");

		ComponentHandle<TransformComponent> transform1 = collider1.GetSiblingComponent<TransformComponent>();
		Assert(rigidBody1.IsValid() && "Transform is invalid. See REsolveDynStcCollision in PhysicsSystem.cpp\n");

		glm::vec2 position = transform1->GetPosition();

		glm::vec3 resolutionVector = *collisionData;

		// calculate elasticity multiplier applied to dynamic object
		float elasticity = collider1->ColliderData().GetSelfElasticity() * collider2->ColliderData().GetAppliedElasticity();

		transform1->SetPosition(glm::vec2(position + static_cast<glm::vec2>(resolutionVector)));

		if (resolutionVector.x)
		{
			rigidBody1->SetVelocity(glm::vec3(rigidBody1->Velocity().x * -elasticity, rigidBody1->Velocity().y, rigidBody1->Velocity().z));
		}
		if (resolutionVector.y)
		{
			rigidBody1->SetVelocity(glm::vec3(rigidBody1->Velocity().x, rigidBody1->Velocity().y * -elasticity, rigidBody1->Velocity().z));
		}
	}
}

void UpdateMovementData(float dt, ComponentHandle<TransformComponent> transform, ComponentHandle<RigidBodyComponent> rigidBody, glm::vec3 velocity, glm::vec3 acceleration)
{
	rigidBody->AddVelocity(rigidBody->Gravity() * dt);
	rigidBody->AddVelocity(acceleration * dt);
	transform->SetPosition(transform->GetPosition() + static_cast<glm::vec2>(velocity) * dt);
}

void DebugDrawAllHitboxes(ComponentMap<DynamicCollider2DComponent> *allDynamicColliders, ComponentMap<StaticCollider2DComponent> *allStaticColliders)
{
	for (auto tDynamiColliderHandle : *allDynamicColliders)
	{
		// get the transform and assert its validity
		ComponentHandle<TransformComponent> transform = tDynamiColliderHandle.GetSiblingComponent<TransformComponent>();
		Assert(transform.IsValid() && "Transform invalid in debug drawing, see DebugDrawAllHitboxes in PhysicsSystem.cpp");

		// get some values about the collider
		float rotation = transform->GetRotation() + tDynamiColliderHandle->ColliderData().GetRotationOffset();
		glm::vec2 position = transform->GetPosition() + tDynamiColliderHandle->ColliderData().GetOffset();
		glm::vec2 dimensions = tDynamiColliderHandle->ColliderData().GetDimensions();

		// draw the collider based on its shape
		if (tDynamiColliderHandle->ColliderData().ColliderIsShape(Collider2D::colliderType::colliderBox))
		{
			DebugGraphic::DrawSquare(position, dimensions, DegreesToRadians(rotation), glm::vec4(1, 0, 1, 1));
		}
		else if (tDynamiColliderHandle->ColliderData().ColliderIsShape(Collider2D::colliderType::colliderCircle))
		{
			DebugGraphic::DrawCircle(position, dimensions.x / 2, glm::vec4(1, 0, 1, 1));
		}
		else if (tDynamiColliderHandle->ColliderData().ColliderIsShape(Collider2D::colliderType::colliderCapsule))
		{
			// get information about the capsule
			CapsuleInformation capsule(position, dimensions, rotation);

			// top circle
			DebugGraphic::DrawCircle(capsule.m_topCircleCenter, capsule.m_circleRadius, glm::vec4(1, 0, 1, 1));
			// rectangle
			DebugGraphic::DrawSquare(capsule.m_boxCenter, capsule.m_boxDimensions, DegreesToRadians(capsule.m_boxRotation), glm::vec4(1, 0, 1, 1));
			// bottom circle
			DebugGraphic::DrawCircle(capsule.m_botCircleCenter, capsule.m_circleRadius, glm::vec4(1, 0, 1, 1));
		}
	}
	for (auto tStaticColliderHandle : *allStaticColliders)
	{
		// get and assert the validity of the transform
		ComponentHandle<TransformComponent> transform = tStaticColliderHandle.GetSiblingComponent<TransformComponent>();
		Assert(transform.IsValid() && "Transform invalid in debug drawing, see DebugDrawAllHitboxes in PhysicsSystem.cpp");

		float rotation = transform->GetRotation() + tStaticColliderHandle->ColliderData().GetRotationOffset();
		glm::vec2 position = transform->GetPosition() + tStaticColliderHandle->ColliderData().GetOffset();
		glm::vec2 dimensions = tStaticColliderHandle->ColliderData().GetDimensions();

		if (tStaticColliderHandle->ColliderData().ColliderIsShape(Collider2D::colliderType::colliderBox))
		{
			DebugGraphic::DrawSquare(position, dimensions, DegreesToRadians(rotation), glm::vec4(.1f, .5f, 1, 1));
		}
		else if (tStaticColliderHandle->ColliderData().ColliderIsShape(Collider2D::colliderType::colliderCircle))
		{
			DebugGraphic::DrawCircle(position, dimensions.x / 2, glm::vec4(.1f, .5f, 1, 1));
		}
		else if (tStaticColliderHandle->ColliderData().ColliderIsShape(Collider2D::colliderType::colliderCapsule))
		{
			// get information about the capsule
			CapsuleInformation capsule(position, dimensions, rotation);

			// top circle
			DebugGraphic::DrawCircle(capsule.m_topCircleCenter, capsule.m_circleRadius, glm::vec4(.1f, .5f, 1, 1));
			// rectangle
			DebugGraphic::DrawSquare(capsule.m_boxCenter, capsule.m_boxDimensions, DegreesToRadians(capsule.m_boxRotation), glm::vec4(.1f, .5f, 1, 1));
			// bottom circle
			DebugGraphic::DrawCircle(capsule.m_botCircleCenter, capsule.m_circleRadius, glm::vec4(.1f, .5f, 1, 1));
		}
	}
}

void ClearAllRecordedCollisions(ComponentMap<DynamicCollider2DComponent> *allDynamicColliders, ComponentMap<StaticCollider2DComponent> *allStaticColliders)
{
	for (auto tDynamiColliderHandle : *allDynamicColliders)
	{
		tDynamiColliderHandle->ColliderData().ClearCollidedLayers();
	}
	for (auto tStaticColliderHandle : *allStaticColliders)
	{
		tStaticColliderHandle->ColliderData().ClearCollidedLayers();
	}
}

void MoveAllDynamicObjects(float dt, ComponentMap<RigidBodyComponent>& rigidBodies)
{
	for (auto tRigidBodyHandle : rigidBodies)
	{
		ComponentHandle<TransformComponent> transform = tRigidBodyHandle.GetSiblingComponent<TransformComponent>();
		if (!transform.IsActive())
		{
			continue;
		}
		Assert(transform.IsValid() && "Invalid Transform from Rigidbody in MoveAllDynamicObjects in PhysicsSystem.cpp");

		// update position, velocity, and acceleration using stored values
		UpdateMovementData(dt, transform, tRigidBodyHandle, tRigidBodyHandle->Velocity(), tRigidBodyHandle->Acceleration());
	}
}

// registers collision between two layers upon each other
void RegisterCollision(Collider2D& collider1, Collider2D& collider2)
{
	collisionLayers collider1Layer = collider1.GetCollisionLayer();
	collisionLayers collider2Layer = collider2.GetCollisionLayer();

	collider1.AddCollidedLayer(collider2Layer);
	collider2.AddCollidedLayer(collider1Layer);
}

void PhysicsSystem::Init()
{

}

void BrettsFunMagicalTestLoop(ComponentMap<DynamicCollider2DComponent> *allDynamicColliders, ComponentMap<StaticCollider2DComponent> *allStaticColliders)
{
	for (auto tDynamiColliderHandle : *allDynamicColliders)
	{
		if (tDynamiColliderHandle->ColliderData().IsCollidingWithLayer(collisionLayers::allyProjectile))
		{
			ComponentHandle<RigidBodyComponent> rigidBody = tDynamiColliderHandle.GetSiblingComponent<RigidBodyComponent>();
			if (!rigidBody.IsActive())
			{
				continue;
			}
			Assert(rigidBody.IsValid() && "Invalid Transform from Rigidbody in ApplyGravityToAllDynamicObjects in PhysicsSystem.cpp");

			rigidBody->AddVelocity(glm::vec3(-.1, .2, 0));
		}
	}
	for (auto tStaticColliderHandle : *allStaticColliders)
	{
		
	}
}

void ApplyGravityToAllDynamicObjects(float dt, ComponentMap<RigidBodyComponent>& rigidBodies)
{
	for (auto tRigidBodyHandle : rigidBodies)
	{
		if (tRigidBodyHandle.IsActive())
		{
			tRigidBodyHandle->AddVelocity(tRigidBodyHandle->Gravity() * dt);
		}
	}
}

void PhysicsSystem::Update(float dt)
{
	// get all rigid bodies
	ComponentMap<RigidBodyComponent> *rigidBodies = GetGameSpace()->GetComponentMap<RigidBodyComponent>();

	// get all dynamic colliders
	ComponentMap<DynamicCollider2DComponent> *allDynamicColliders = GetGameSpace()->GetComponentMap<DynamicCollider2DComponent>();

	// get all static colliders
	ComponentMap<StaticCollider2DComponent> *allStaticColliders = GetGameSpace()->GetComponentMap<StaticCollider2DComponent>();

	// clear out the recorded collision layers so the recording will be accurate to this frame
	ClearAllRecordedCollisions(allDynamicColliders, allStaticColliders);

	// update the position and velocity of all objects according to their velocity and acceleration
	MoveAllDynamicObjects(dt, *rigidBodies);

	


	for (auto iDynamicCollider = allDynamicColliders->begin(); iDynamicCollider != allDynamicColliders->end(); ++iDynamicCollider)
	{
		ComponentHandle<DynamicCollider2DComponent> dynamicCollider = *iDynamicCollider;

		// get the transform from the same gameobject, and leave the loop if it isn't valid
		ComponentHandle<RigidBodyComponent> tRigidBodyHandle = dynamicCollider.GetSiblingComponent<RigidBodyComponent>();
		ComponentHandle<TransformComponent> transform = dynamicCollider.GetSiblingComponent<TransformComponent>();

		if (!transform.IsActive())
		{
			continue;
		}

		// if transform and collider are valid, collide it with things
		if (transform.IsValid() && dynamicCollider.IsValid())
		{
			glm::vec3 resolutionVector(0);

			glm::vec3 collidedAcceleration = tRigidBodyHandle->Acceleration();

			// loop through all static colliders
			for (auto tStaticColliderHandle : *allStaticColliders)
			{
				if (!tStaticColliderHandle.IsActive())
				{
					continue;
				}

				// get the colliders out of the objects
				Collider2D collider1 = dynamicCollider->ColliderData();
				Collider2D collider2 = tStaticColliderHandle->ColliderData();

				if (!collider1.GetCollisionLayer().LayersCollide(collider2.GetCollisionLayer()))
				{
					continue;
				}

				ComponentHandle<TransformComponent> otherTransform = tStaticColliderHandle.GetSiblingComponent<TransformComponent>();
				Assert(otherTransform.IsValid() && "Some static object's returned an invalid transform in PhysicsSysterm::Update in PhysicsSystem.cpp");

				float object1Rotation = transform->GetRotation() + collider1.GetRotationOffset();
				float object2Rotation = otherTransform->GetRotation() + collider2.GetRotationOffset();

				int object1Shape = collider1.GetColliderShape();
				int object2Shape = collider2.GetColliderShape();

				// figure out what type of collision is taking place
				bool boxBoxCollision = object1Shape == Collider2D::colliderType::colliderBox && object2Shape == Collider2D::colliderType::colliderBox;
				bool circleBoxCollision = object1Shape == Collider2D::colliderType::colliderBox && object2Shape == Collider2D::colliderType::colliderCircle ||
				                          object1Shape == Collider2D::colliderType::colliderCircle && object2Shape == Collider2D::colliderType::colliderBox;
				bool circleCircleCollision = object1Shape == Collider2D::colliderType::colliderCircle && object2Shape == Collider2D::colliderType::colliderCircle;
				bool capsuleBoxCollision = object1Shape == Collider2D::colliderType::colliderBox && object2Shape == Collider2D::colliderType::colliderCapsule ||
					                       object1Shape == Collider2D::colliderType::colliderCapsule && object2Shape == Collider2D::colliderType::colliderBox;
				bool capsuleCircleCollision = object1Shape == Collider2D::colliderType::colliderCircle && object2Shape == Collider2D::colliderType::colliderCapsule ||
					                          object1Shape == Collider2D::colliderType::colliderCapsule && object2Shape == Collider2D::colliderType::colliderCircle;
				bool capsuleCapsuleCollision = object1Shape == Collider2D::colliderType::colliderCapsule && object2Shape == Collider2D::colliderType::colliderCapsule;

				if (boxBoxCollision)
				{
					// check for collision on non-rotated boxes
					if (object1Rotation == 0 && object2Rotation == 0)
					{
						resolutionVector = Collision_AABBToAABB(transform, collider1, otherTransform, collider2);
					}
					else // check for collision on rotated boxes
					{
						resolutionVector = Collision_SAT(transform, collider1, otherTransform, collider2);
					}
				}
				else if(circleBoxCollision)
				{
					// the centers of the colliders
					glm::vec2 center1 = transform->GetPosition() + static_cast<glm::vec2>(dynamicCollider->ColliderData().GetOffset());
					glm::vec2 center2 = otherTransform->GetPosition() + static_cast<glm::vec2>(tStaticColliderHandle->ColliderData().GetOffset());

					// if the object1 is the circle
					if (object1Shape == Collider2D::colliderType::colliderCircle)
					{
						// the radius of the circle is object1
						float radius = dynamicCollider->ColliderData().GetDimensions().x / 2.0f;
						// the dimensions of the box are object2
						glm::vec2 dimensions = tStaticColliderHandle->ColliderData().GetDimensions();
						// get the rotation of the box
						float rotation = tStaticColliderHandle->ColliderData().GetRotationOffset() + otherTransform->GetRotation();
						// get the corners of the rectangle
						BoxCorners rectangle(center2, dimensions, rotation);
						// get the resolution vector of the collision
						resolutionVector = Collision_SAT_CircleBox(center1, radius, rectangle);
					}
					else // if object2 is the circle
					{
						// the radius of the circle are object2
						float radius = tStaticColliderHandle->ColliderData().GetDimensions().x / 2.0f;
						// the dimensions of the box are object1
						glm::vec2 dimensions = dynamicCollider->ColliderData().GetDimensions();
						// get the rotation of the box
						float rotation = dynamicCollider->ColliderData().GetRotationOffset() + transform->GetRotation();
						// get the corners of the rectangle
						BoxCorners rectangle(center2, dimensions, rotation);
						// get the resolution vector of the collision
						resolutionVector = Collision_SAT_BoxCircle(rectangle, center2, radius);
					}
				}
				else if (circleCircleCollision)
				{
					// the centers of the colliders
					glm::vec2 center1 = transform->GetPosition() + static_cast<glm::vec2>(dynamicCollider->ColliderData().GetOffset());
					glm::vec2 center2 = otherTransform->GetPosition() + static_cast<glm::vec2>(tStaticColliderHandle->ColliderData().GetOffset());

					// the radii of the colliders
					float radius1 = dynamicCollider->ColliderData().GetDimensions().x / 2.0f;
					float radius2 = tStaticColliderHandle->ColliderData().GetDimensions().x / 2.0f;

					resolutionVector = Collision_CircleCircle(center1, radius1, center2, radius2);
				}
				else if (capsuleBoxCollision)
				{
					// the centers of the colliders
					glm::vec2 center1 = transform->GetPosition() + static_cast<glm::vec2>(dynamicCollider->ColliderData().GetOffset());
					glm::vec2 center2 = otherTransform->GetPosition() + static_cast<glm::vec2>(tStaticColliderHandle->ColliderData().GetOffset());

					// if object 1 is the capsule
					if (object1Shape == Collider2D::colliderType::colliderCapsule)
					{
						float capsuleRotation = transform->GetRotation() + dynamicCollider->ColliderData().GetRotationOffset();
						float boxRotation = otherTransform->GetRotation() + tStaticColliderHandle->ColliderData().GetRotationOffset();

						BoxCorners box(center2, tStaticColliderHandle->ColliderData().GetDimensions(), boxRotation);

						resolutionVector = Collision_SAT_CapsuleBox(center1, dynamicCollider->ColliderData().GetDimensions(), capsuleRotation, box);
					}
					else // else object 2 is the capsule
					{
						float capsuleRotation = otherTransform->GetRotation() + tStaticColliderHandle->ColliderData().GetRotationOffset();
						float boxRotation = transform->GetRotation() + dynamicCollider->ColliderData().GetRotationOffset();

						BoxCorners box(center1, dynamicCollider->ColliderData().GetDimensions(), boxRotation);

						resolutionVector = Collision_SAT_BoxCapsule(box, center2, tStaticColliderHandle->ColliderData().GetDimensions(), capsuleRotation);
					}
				}
				else if (capsuleCircleCollision)
				{
					// the centers of the colliders
					glm::vec2 center1 = transform->GetPosition() + static_cast<glm::vec2>(dynamicCollider->ColliderData().GetOffset());
					glm::vec2 center2 = otherTransform->GetPosition() + static_cast<glm::vec2>(tStaticColliderHandle->ColliderData().GetOffset());

					// if object 1 is the capsule
					if (object1Shape == Collider2D::colliderType::colliderCapsule)
					{
						// capsule information
						glm::vec2 capsuleDimensions = dynamicCollider->ColliderData().GetDimensions();
						float capsuleRotation = transform->GetRotation() + dynamicCollider->ColliderData().GetRotationOffset();

						// circle information
						float circleRadius = tStaticColliderHandle->ColliderData().GetDimensions().x * 0.5f;

						resolutionVector = Collision_SAT_CapsuleCircle(center1, capsuleDimensions, capsuleRotation, center2, circleRadius);
					}
					else // else object 2 is the capsule
					{
						// circle information
						float circleRadius = dynamicCollider->ColliderData().GetDimensions().x * 0.5f;

						// capsule information
						glm::vec2 capsuleDimensions = tStaticColliderHandle->ColliderData().GetDimensions();
						float capsuleRotation = otherTransform->GetRotation() + tStaticColliderHandle->ColliderData().GetRotationOffset();

						resolutionVector = Collision_SAT_CircleCapsule(center1, circleRadius, center2, capsuleDimensions, capsuleRotation);
					}
				}
				else if (capsuleCapsuleCollision)
				{
					// capsule1 information
					glm::vec2 c1center = transform->GetPosition() + static_cast<glm::vec2>(dynamicCollider->ColliderData().GetOffset());
					glm::vec2 c1Dimensions = dynamicCollider->ColliderData().GetDimensions();
					float c1Rotation = transform->GetRotation() + dynamicCollider->ColliderData().GetRotationOffset();

					// capsule2 information
					glm::vec2 c2center = otherTransform->GetPosition() + static_cast<glm::vec2>(tStaticColliderHandle->ColliderData().GetOffset());
					glm::vec2 c2Dimensions = tStaticColliderHandle->ColliderData().GetDimensions();
					float c2Rotation = otherTransform->GetRotation() + tStaticColliderHandle->ColliderData().GetRotationOffset();

					resolutionVector = Collision_SAT_CapsuleCapsule(c1center, c1Dimensions, c1Rotation, c2center, c2Dimensions, c2Rotation);
				}

				// if there was a collision, resolve it
				if (resolutionVector.x || resolutionVector.y)
				{
					// register collision between the layers
					RegisterCollision(dynamicCollider->ColliderData(), tStaticColliderHandle->ColliderData());
					// resolve the collision
					ResolveDynStcCollision(dt, &resolutionVector, dynamicCollider, tStaticColliderHandle);

					// Call the collision function on scripts.
					ComponentHandle<ScriptComponent> script1 = dynamicCollider.GetSiblingComponent<ScriptComponent>();
					if (script1.IsValid())
					{
						script1->CallCollision(tStaticColliderHandle.GetGameObject());
					}
					ComponentHandle<ScriptComponent> script2 = tStaticColliderHandle.GetSiblingComponent<ScriptComponent>();
					if (script2.IsValid())
					{
						script2->CallCollision(dynamicCollider.GetGameObject());
					}
				}
			}

			// loop through all dynamic colliders after the first one
			auto iDynamiColliderHandle = iDynamicCollider;
			for (++iDynamiColliderHandle; iDynamiColliderHandle != allDynamicColliders->end(); ++iDynamiColliderHandle)
			{
				ComponentHandle<DynamicCollider2DComponent> tDynamiColliderHandle = *iDynamiColliderHandle;

				if (!tDynamiColliderHandle.IsActive())
				{
					continue;
				}

				// get the colliders out of the objects
				Collider2D collider1 = dynamicCollider->ColliderData();
				Collider2D collider2 = tDynamiColliderHandle->ColliderData();

				if (dynamicCollider == tDynamiColliderHandle)
				{
					continue;
				}

				if (!collider1.GetCollisionLayer().LayersCollide(collider2.GetCollisionLayer()))
				{
					continue;
				}

				ComponentHandle<TransformComponent> otherTransform = tDynamiColliderHandle.GetSiblingComponent<TransformComponent>();
				Assert(otherTransform.IsValid() && "Invalid transform on collider, see PhysicsSystem::Update in PhysicsSystem.cpp");

				float object1Rotation = transform->GetRotation() + collider1.GetRotationOffset();
				float object2Rotation = otherTransform->GetRotation() + collider2.GetRotationOffset();

				int object1Shape = collider1.GetColliderShape();
				int object2Shape = collider2.GetColliderShape();

				bool boxBoxCollision = object1Shape == Collider2D::colliderType::colliderBox && object2Shape == Collider2D::colliderType::colliderBox;
				bool circleBoxCollision = object1Shape == Collider2D::colliderType::colliderBox && object2Shape == Collider2D::colliderType::colliderCircle ||
					object1Shape == Collider2D::colliderType::colliderCircle && object2Shape == Collider2D::colliderType::colliderBox;
				bool circleCircleCollision = object1Shape == Collider2D::colliderType::colliderCircle && object2Shape == Collider2D::colliderType::colliderCircle;
				bool capsuleBoxCollision = object1Shape == Collider2D::colliderType::colliderBox && object2Shape == Collider2D::colliderType::colliderCapsule ||
					object1Shape == Collider2D::colliderType::colliderCapsule && object2Shape == Collider2D::colliderType::colliderBox;
				bool capsuleCircleCollision = object1Shape == Collider2D::colliderType::colliderCircle && object2Shape == Collider2D::colliderType::colliderCapsule ||
					object1Shape == Collider2D::colliderType::colliderCapsule && object2Shape == Collider2D::colliderType::colliderCircle;
				bool capsuleCapsuleCollision = object1Shape == Collider2D::colliderType::colliderCapsule && object2Shape == Collider2D::colliderType::colliderCapsule;

				// check for collision on non-rotated boxes
				if (boxBoxCollision)
				{
					if (object1Rotation == 0 && object2Rotation == 0)
					{
						resolutionVector = Collision_AABBToAABB(transform, collider1, otherTransform, collider2);
					}
					else // check for collision on rotated boxes
					{
						resolutionVector = Collision_SAT(transform, collider1, otherTransform, collider2);
					}
				}
				else if (circleBoxCollision)
				{
					// the centers of the colliders
					glm::vec2 center1 = transform->GetPosition() + static_cast<glm::vec2>(dynamicCollider->ColliderData().GetOffset());
					glm::vec2 center2 = otherTransform->GetPosition() + static_cast<glm::vec2>(tDynamiColliderHandle->ColliderData().GetOffset());

					// if the object1 is the circle
					if (object1Shape == Collider2D::colliderType::colliderCircle)
					{
						// the radius of the circle is object1
						float radius = dynamicCollider->ColliderData().GetDimensions().x / 2.0f;
						// the dimensions of the box are object2
						glm::vec2 dimensions = tDynamiColliderHandle->ColliderData().GetDimensions();
						// get the rotation of the box
						float rotation = tDynamiColliderHandle->ColliderData().GetRotationOffset() + otherTransform->GetRotation();

						BoxCorners rectangle(center2, dimensions, rotation);

						resolutionVector = Collision_SAT_CircleBox(center1, radius, rectangle);
					}
					else // if object2 is the circle
					{
						// the radius of the circle are object2
						float radius = tDynamiColliderHandle->ColliderData().GetDimensions().x / 2.0f;
						// the dimensions of the box are object1
						glm::vec2 dimensions = dynamicCollider->ColliderData().GetDimensions();
						// get the rotation of the box
						float rotation = dynamicCollider->ColliderData().GetRotationOffset() + transform->GetRotation();

						BoxCorners rectangle(center2, dimensions, rotation);

						resolutionVector = Collision_SAT_BoxCircle(rectangle, center2, radius);
					}
				}
				else if (circleCircleCollision)
				{
					// the centers of the colliders
					glm::vec2 center1 = transform->GetPosition() + static_cast<glm::vec2>(dynamicCollider->ColliderData().GetOffset());
					glm::vec2 center2 = otherTransform->GetPosition() + static_cast<glm::vec2>(tDynamiColliderHandle->ColliderData().GetOffset());

					// the radii of the colliders
					float radius1 = dynamicCollider->ColliderData().GetDimensions().x / 2.0f;
					float radius2 = tDynamiColliderHandle->ColliderData().GetDimensions().x / 2.0f;

					resolutionVector = Collision_CircleCircle(center1, radius1, center2, radius2);
				}
				else if (capsuleBoxCollision)
				{
					// the centers of the colliders
					glm::vec2 center1 = transform->GetPosition() + static_cast<glm::vec2>(dynamicCollider->ColliderData().GetOffset());
					glm::vec2 center2 = otherTransform->GetPosition() + static_cast<glm::vec2>(tDynamiColliderHandle->ColliderData().GetOffset());

					// if object 1 is the capsule
					if (object1Shape == Collider2D::colliderType::colliderCapsule)
					{
						float capsuleRotation = transform->GetRotation() + dynamicCollider->ColliderData().GetRotationOffset();
						float boxRotation = otherTransform->GetRotation() + tDynamiColliderHandle->ColliderData().GetRotationOffset();

						BoxCorners box(center2, tDynamiColliderHandle->ColliderData().GetDimensions(), boxRotation);

						resolutionVector = Collision_SAT_CapsuleBox(center1, dynamicCollider->ColliderData().GetDimensions(), capsuleRotation, box);
					}
					else // else object 2 is the capsule
					{
						float capsuleRotation = otherTransform->GetRotation() + tDynamiColliderHandle->ColliderData().GetRotationOffset();
						float boxRotation = transform->GetRotation() + dynamicCollider->ColliderData().GetRotationOffset();

						BoxCorners box(center1, dynamicCollider->ColliderData().GetDimensions(), boxRotation);

						resolutionVector = Collision_SAT_BoxCapsule(box, center2, tDynamiColliderHandle->ColliderData().GetDimensions(), capsuleRotation);
					}
				}
				else if (capsuleCircleCollision)
				{
					// the centers of the colliders
					glm::vec2 center1 = transform->GetPosition() + static_cast<glm::vec2>(dynamicCollider->ColliderData().GetOffset());
					glm::vec2 center2 = otherTransform->GetPosition() + static_cast<glm::vec2>(tDynamiColliderHandle->ColliderData().GetOffset());

					// if object 1 is the capsule
					if (object1Shape == Collider2D::colliderType::colliderCapsule)
					{
						// capsule information
						glm::vec2 capsuleDimensions = dynamicCollider->ColliderData().GetDimensions();
						float capsuleRotation = transform->GetRotation() + dynamicCollider->ColliderData().GetRotationOffset();

						// circle information
						float circleRadius = tDynamiColliderHandle->ColliderData().GetDimensions().x * 0.5f;

						resolutionVector = Collision_SAT_CapsuleCircle(center1, capsuleDimensions, capsuleRotation, center2, circleRadius);
					}
					else // else object 2 is the capsule
					{
						// circle information
						float circleRadius = dynamicCollider->ColliderData().GetDimensions().x * 0.5f;

						// capsule information
						glm::vec2 capsuleDimensions = tDynamiColliderHandle->ColliderData().GetDimensions();
						float capsuleRotation = otherTransform->GetRotation() + tDynamiColliderHandle->ColliderData().GetRotationOffset();

						resolutionVector = Collision_SAT_CircleCapsule(center1, circleRadius, center2, capsuleDimensions, capsuleRotation);
					}
				}
				else if (capsuleCapsuleCollision)
				{
					// capsule1 information
					glm::vec2 c1center = transform->GetPosition() + static_cast<glm::vec2>(dynamicCollider->ColliderData().GetOffset());
					glm::vec2 c1Dimensions = dynamicCollider->ColliderData().GetDimensions();
					float c1Rotation = transform->GetRotation() + dynamicCollider->ColliderData().GetRotationOffset();

					// capsule2 information
					glm::vec2 c2center = otherTransform->GetPosition() + static_cast<glm::vec2>(tDynamiColliderHandle->ColliderData().GetOffset());
					glm::vec2 c2Dimensions = tDynamiColliderHandle->ColliderData().GetDimensions();
					float c2Rotation = otherTransform->GetRotation() + tDynamiColliderHandle->ColliderData().GetRotationOffset();

					resolutionVector = Collision_SAT_CapsuleCapsule(c1center, c1Dimensions, c1Rotation, c2center, c2Dimensions, c2Rotation);
				}


				// if there was a collision, resolve it
				if (resolutionVector.x || resolutionVector.y)
				{
					// register collision between the layers
					RegisterCollision(dynamicCollider->ColliderData(), tDynamiColliderHandle->ColliderData());
					// resolve the collision
					ResolveDynDynCollision(dt, &resolutionVector, dynamicCollider, transform, tDynamiColliderHandle, otherTransform);

					// Call the collision function on scripts.
					ComponentHandle<ScriptComponent> script1 = dynamicCollider.GetSiblingComponent<ScriptComponent>();
					if (script1.IsValid())
					{
						script1->CallCollision(tDynamiColliderHandle.GetGameObject());
					}
					ComponentHandle<ScriptComponent> script2 = tDynamiColliderHandle.GetSiblingComponent<ScriptComponent>();
					if (script2.IsValid())
					{
						script2->CallCollision(dynamicCollider.GetGameObject());
					}
				}
			}
		}
	}

	//Add Gravity to objects
	//ApplyGravityToAllDynamicObjects(dt, *rigidBodies);

/************************** TEST STUFF **************************/
	//CollisionLayerTestFuction();

	//const int numDir = collisionLayers::numLayers * 3;

	//glm::vec2 castPosition(7, 2);

	//glm::vec2 direction[numDir];

	//float range = 4;

	//for (int i = 0; i < numDir; ++i)
	//{
	//	direction[i] = glm::vec2(-1.0f + ((2.0f * i) / numDir), -1);
	//}

	//for (int i = 0; i < numDir; ++i)
	//{
	//	collisionLayers layer = static_cast<collisionLayers>(1 << (i % collisionLayers::numLayers));

	//	Raycast testCast(allDynamicColliders, allStaticColliders, castPosition, direction[i], range, layer);

	//	float colorval = (1.0f / collisionLayers::numLayers) * ((i % collisionLayers::numLayers) +1);

	//	//testCast.Draw(glm::vec4(colorval, colorval, colorval, 1));
	//}

	/*glm::vec2 testPoint(1, 0);
	glm::vec2 pointEscape = CollidePointOnLayer(allDynamicColliders, allStaticColliders, testPoint);
	testPoint += pointEscape;
	DrawSmallBoxAtPosition(testPoint);*/

	//BrettsFunMagicalTestLoop(allDynamicColliders, allStaticColliders);


/************************ END TEST STUFF ************************/

	if (debugShowHitboxes)
	{
		DebugDrawAllHitboxes(allDynamicColliders, allStaticColliders);
	}
}

SystemBase *PhysicsSystem::NewDuplicate()
{
	return new PhysicsSystem(*this);
}

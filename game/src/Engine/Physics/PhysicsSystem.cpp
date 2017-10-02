/*
FILE: PhysicsSystem.cpp
PRIMARY AUTHOR: Brett Schiff

Handles Movement, Colliders, Collision Detection, and Collision Resolution

Copyright � 2017 DigiPen (USA) Corporation.
*/
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

#include "PhysicsSystem.h"
#include "RigidBody.h"
#include "Collider2D.h"
#include "../../graphics/DebugGraphic.h"

bool debugShowHitboxes = true;

void debugDisplayHitboxes(bool hitboxesShown)
{
	debugShowHitboxes = hitboxesShown;
}

class ColliderBox
{
public:
	ColliderBox(const glm::vec3& center, const glm::vec3& dimensions, const ComponentHandle<RigidBodyComponent> rigidBody, float rotation);

	friend std::ostream& operator<<(std::ostream& ostream, const ColliderBox& colliderBox);

	glm::vec3 m_topRight;
	glm::vec3 m_botLeft;
	float m_rotation;
};

std::ostream& operator<<(std::ostream& ostream, const ColliderBox& colliderBox)
{
	ostream << "Top Right Corner: (" << colliderBox.m_topRight.x << ", " << colliderBox.m_topRight.y << ")" << std::endl;
	ostream << "Bot Left  Corner: (" << colliderBox.m_botLeft.x << ", " << colliderBox.m_botLeft.y << ")" << std::endl;


	return ostream;
}

ColliderBox::ColliderBox(const glm::vec3& center, const glm::vec3& dimensions, ComponentHandle<RigidBodyComponent> rigidBody, float rotation)
{
	glm::vec3 centerWithVelocity = center;
	if (rigidBody.IsValid())
	{
		centerWithVelocity += rigidBody->Velocity();
	}

	// if the box is axis-aligned, the calculation can be done ignoring angles
	if (rotation == 0)
	{
		m_topRight = centerWithVelocity + (.5f * dimensions);
		m_botLeft = centerWithVelocity - (.5f * dimensions);
		m_rotation = 0;
	}
	else
	{
		m_rotation = rotation;

		// calculate the top right corner
		m_topRight.x = centerWithVelocity.x + (dimensions.x * 0.5f * cos(m_rotation)) - (dimensions.y * 0.5f * sin(m_rotation));
		m_topRight.y = centerWithVelocity.y + (dimensions.x * 0.5f * sin(m_rotation)) + (dimensions.y * 0.5f * cos(m_rotation));

		// calculate the bottom left corner
		m_botLeft.x = centerWithVelocity.x - (dimensions.x * 0.5f * cos(m_rotation)) + (dimensions.y * 0.5f * sin(m_rotation));
		m_botLeft.y = centerWithVelocity.y - (dimensions.x * 0.5f * sin(m_rotation)) - (dimensions.y * 0.5f * cos(m_rotation));
	}
}

glm::vec3 Collision_AABBToAABB(ComponentHandle<TransformComponent>& AABB1Transform, Collider2D& AABB1Collider, ComponentHandle<TransformComponent>& AABB2Transform, Collider2D& AABB2Collider)
{
	ColliderBox Box1(AABB1Transform->Position(), AABB1Collider.GetDimensions(), AABB1Transform.GetSiblingComponent<RigidBodyComponent>(), AABB1Transform->Rotation() + AABB1Collider.GetRotationOffset());
	ColliderBox Box2(AABB2Transform->Position(), AABB2Collider.GetDimensions(), AABB2Transform.GetSiblingComponent<RigidBodyComponent>(), AABB2Transform->Rotation() + AABB2Collider.GetRotationOffset());

	glm::vec3 escapeVector(0);
	glm::vec3 minValue(0);

	if (Box1.m_topRight.x < Box2.m_botLeft.x)
	{
		return glm::vec3(0);
	}
	else
	{
		minValue.x = Box1.m_topRight.x - Box2.m_botLeft.x;
	}
	if (Box1.m_topRight.y < Box2.m_botLeft.y)
	{
		return glm::vec3(0);
	}
	else
	{
		minValue.y = Box1.m_topRight.y - Box2.m_botLeft.y;
	}
	if (Box1.m_botLeft.x > Box2.m_topRight.x)
	{
		return glm::vec3(0);
	}
	else
	{
		if (abs(Box1.m_botLeft.x - Box2.m_topRight.x) < abs(minValue.x))
		{
			minValue.x = Box1.m_botLeft.x - Box2.m_topRight.x;
		}
	}
	if (Box1.m_botLeft.y > Box2.m_topRight.y)
	{
		return glm::vec3(0);
	}
	else
	{
		if (abs(Box1.m_botLeft.y - Box2.m_topRight.y) < abs(minValue.y))
		{
			minValue.y = Box1.m_botLeft.y - Box2.m_topRight.y;
		}
	}

	// if the resolution needs to be primarily along the x axis
	if (abs(minValue.x) < abs(minValue.y))
	{
		escapeVector.x = minValue.x;
	}
	else // if the resolution needs to be primarily along the y axis
	{
		escapeVector.y = minValue.y;
	}

	return escapeVector;
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

void ResolveDynDynCollision(glm::vec3* collisionData, ComponentHandle<DynamicCollider2DComponent> collider1, ComponentHandle<TransformComponent> transform1, ComponentHandle<DynamicCollider2DComponent> collider2, ComponentHandle<TransformComponent> transform2)
{
	ComponentHandle<RigidBodyComponent> rigidBody1 = collider1.GetSiblingComponent<RigidBodyComponent>();
	ComponentHandle<RigidBodyComponent> rigidBody2 = collider2.GetSiblingComponent<RigidBodyComponent>();
	// make sure rigidbodies were successfully retrieved
	assert(rigidBody1.IsValid() && rigidBody2.IsValid() && "Rigidbody(s) invalid. See ResolveDynDynCollision in PhysicsSystem.cpp\n");

	glm::vec3 resolutionVector = *collisionData;
	glm::vec3 dividingVector((*collisionData).y, -(*collisionData).x, 0);

	//std::cout << dividingVector.x << " " << dividingVector.y << "\n";

	float a = dividingVector.x;
	float b = dividingVector.y;
	float a2 = dividingVector.x * dividingVector.x;
	float b2 = dividingVector.y * dividingVector.y;

	// get the matrix for reflecting over dividing vector
	glm::mat3 refMtrx(1-((2*b2)/(a2+b2)), (2*a*b)/(a2+b2), 0,
					 ((2*a*b)/(a2+b2)), ((2*b2)/(a2+b2)) - 1, 0,
					 0, 0, 1
					 );

	printAMatrix(refMtrx);

	// check to see if the objects are moving in the same direction
	float xCompare = rigidBody1->Velocity().x / rigidBody2->Velocity().x;
	float yCompare = rigidBody1->Velocity().y / rigidBody2->Velocity().y;

	// if they are not going in the same direction
	if (xCompare < 0 || yCompare < 0)
	{
		/*std::cout << "Before:\n";
		std::cout << rigidBody1->Velocity().x << ", " << rigidBody1->Velocity().y << std::endl;
		std::cout << rigidBody2->Velocity().x << ", " << rigidBody2->Velocity().y << std::endl;*/

		rigidBody1->SetVelocity(refMtrx * rigidBody1->Velocity());
		rigidBody2->SetVelocity(refMtrx * rigidBody2->Velocity());

		/*std::cout << "After:\n";
		std::cout << rigidBody1->Velocity().x << ", " << rigidBody1->Velocity().y << std::endl;
		std::cout << rigidBody2->Velocity().x << ", " << rigidBody2->Velocity().y << std::endl;*/
	}
	else // if they are going in the same direction
	{
		float obj1SquaredMagnitude = (rigidBody1->Velocity().x * rigidBody1->Velocity().x) + (rigidBody1->Velocity().y * rigidBody1->Velocity().y);
		float obj2SquaredMagnitude = (rigidBody2->Velocity().x * rigidBody2->Velocity().x) + (rigidBody2->Velocity().y * rigidBody2->Velocity().y);

		if (obj1SquaredMagnitude < obj2SquaredMagnitude)
		{
			rigidBody1->SetVelocity(rigidBody1->Velocity());
			rigidBody2->SetVelocity(refMtrx * rigidBody2->Velocity());
		}
		else
		{
			rigidBody1->SetVelocity(refMtrx * rigidBody1->Velocity());
			rigidBody2->SetVelocity(rigidBody2->Velocity());
		}
	}
}

void ResolveDynStcCollision(glm::vec3* collisionData, ComponentHandle<DynamicCollider2DComponent> collider1, ComponentHandle<StaticCollider2DComponent> collider2)
{
	ComponentHandle<RigidBodyComponent> rigidBody1 = collider1.GetSiblingComponent<RigidBodyComponent>();
	// make sure rigidbodies were successfully retrieved
	assert(rigidBody1.IsValid() && "Rigidbody/Transform invalid. See ResolveDynStcCollision in PhysicsSystem.cpp\n");

	glm::vec3 resolutionVector = *collisionData;

	//rigidBody1->Velocity() -= resolutionVector;

	if (resolutionVector.x)
	{
		rigidBody1->SetVelocity(glm::vec3(rigidBody1->Velocity().x - resolutionVector.x, rigidBody1->Velocity().y, rigidBody1->Velocity().z));
	}
	if (resolutionVector.y)
	{
		rigidBody1->SetVelocity(glm::vec3(rigidBody1->Velocity().x, 0/*rigidBody1->Velocity().y - resolutionVector.y*/, rigidBody1->Velocity().z));
	}
}

void UpdateMovementData(ComponentHandle<TransformComponent> transform, ComponentHandle<RigidBodyComponent> rigidBody, glm::vec3 velocity, glm::vec3 acceleration)
{
	transform->Position() += velocity;
	rigidBody->AddVelocity(acceleration);

}

void DebugDrawAllHitboxes(ComponentMap<DynamicCollider2DComponent> *allDynamicColliders, ComponentMap<StaticCollider2DComponent> *allStaticColliders)
{
	for (auto tDynamiColliderHandle : *allDynamicColliders)
	{
		ComponentHandle<TransformComponent> transform = tDynamiColliderHandle.GetSiblingComponent<TransformComponent>();
		assert(transform.IsValid() && "Transform invalid in debug drawing, see DebugDrawAllHitboxes in PhysicsSystem.cpp");

		DebugGraphic::DrawShape(transform->GetPosition(), tDynamiColliderHandle->ColliderData().GetDimensions(), transform->GetRotation(), glm::vec4(1, 0, 1, 1));
	}
	for (auto tStaticColliderHandle : *allStaticColliders)
	{
		ComponentHandle<TransformComponent> transform = tStaticColliderHandle.GetSiblingComponent<TransformComponent>();
		assert(transform.IsValid() && "Transform invalid in debug drawing, see DebugDrawAllHitboxes in PhysicsSystem.cpp");

		DebugGraphic::DrawShape(transform->GetPosition(), tStaticColliderHandle->ColliderData().GetDimensions(), transform->GetRotation(), glm::vec4(1, 0, 1, 1));
	}
}

void PhysicsSystem::Init()
{

}

void PhysicsSystem::Update(float dt)
{
	// get all rigid bodies
	ComponentMap<RigidBodyComponent> *rigidBodies = GetGameSpace()->GetComponentMap<RigidBodyComponent>();

	// get all dynamic colliders
	ComponentMap<DynamicCollider2DComponent> *allDynamicColliders = GetGameSpace()->GetComponentMap<DynamicCollider2DComponent>();

	// get all static colliders
	ComponentMap<StaticCollider2DComponent> *allStaticColliders = GetGameSpace()->GetComponentMap<StaticCollider2DComponent>();

	if (debugShowHitboxes)
	{
		DebugDrawAllHitboxes(allDynamicColliders, allStaticColliders);
	}


	for (auto tRigidBodyHandle : *rigidBodies)
	{
		// get the transform from the same gameobject, and leave the loop if it isn't valid
		ComponentHandle<TransformComponent> transform = tRigidBodyHandle.GetSiblingComponent<TransformComponent>();
		ComponentHandle<DynamicCollider2DComponent> dynamicCollider = tRigidBodyHandle.GetSiblingComponent<DynamicCollider2DComponent>();
		
		// if transform and collider are valid, collide it with things
		if (transform.IsValid() && dynamicCollider.IsValid())
		{
			glm::vec3 resolutionVector(0);

			glm::vec3 collidedAcceleration = tRigidBodyHandle->Acceleration();

			// loop through all dynamic colliders
			for (auto tDynamiColliderHandle : *allDynamicColliders)
			{
				if (dynamicCollider == tDynamiColliderHandle)
				{
					continue;
				}

				ComponentHandle<TransformComponent> otherTransform = tDynamiColliderHandle.GetSiblingComponent<TransformComponent>();
				assert(otherTransform.IsValid() && "Invalid transform on collider, see PhysicsSystem::Update in PhysicsSystem.cpp");

				// check for collision
				resolutionVector = Collision_AABBToAABB(transform, dynamicCollider->ColliderData(), otherTransform, tDynamiColliderHandle->ColliderData());
			
				// if there was a collision, resolve it
				if (resolutionVector.x || resolutionVector.y)
				{
					ResolveDynDynCollision(&resolutionVector, dynamicCollider, transform, tDynamiColliderHandle, otherTransform);
				}
			}

			// loop through all static colliders
			for (auto tStaticColliderHandle : *allStaticColliders)
			{
				resolutionVector = Collision_AABBToAABB(transform, dynamicCollider->ColliderData(), tStaticColliderHandle.GetSiblingComponent<TransformComponent>(), tStaticColliderHandle->ColliderData());

				// if there was a collision, resolve it
				if (resolutionVector.x || resolutionVector.y)
				{
					ResolveDynStcCollision(&resolutionVector, dynamicCollider, tStaticColliderHandle);
				}
			}

			// update position, velocity, and acceleration using stored values
			UpdateMovementData(transform, tRigidBodyHandle, tRigidBodyHandle->Velocity(), tRigidBodyHandle->Acceleration());	
		}
		// if transform is valid and dynamic collider isnt, only update movement
		else if (transform.IsValid() && !dynamicCollider.IsValid())
		{
			// update position, velocity, and acceleration using stored values
			UpdateMovementData(transform, tRigidBodyHandle, tRigidBodyHandle->Velocity(), tRigidBodyHandle->Acceleration());
		}
	}
}
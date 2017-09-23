/*
FILE: PhysicsSystem.cpp
PRIMARY AUTHOR: Brett Schiff

Handles Movement, Colliders, Collision Detection, and Collision Resolution

Copyright © 2017 DigiPen (USA) Corporation.
*/
#include <math.h>
#include <iostream>

#include "PhysicsSystem.h"
#include "RigidBody.h"
#include "Collider2D.h"


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

	if (abs(minValue.x) < abs(minValue.y))
	{
		escapeVector.x = minValue.x;
	}
	else
	{
		escapeVector.y = minValue.y;
	}

	return escapeVector;
}


void ResolveDynDynCollision(glm::vec3* collisionData, ComponentHandle<DynamicCollider2DComponent> collider1, ComponentHandle<DynamicCollider2DComponent> collider2)
{
	ComponentHandle<RigidBodyComponent> rigidBody1 = collider1.GetSiblingComponent<RigidBodyComponent>();
	ComponentHandle<RigidBodyComponent> rigidBody2 = collider2.GetSiblingComponent<RigidBodyComponent>();
	// make sure rigidbodies were successfully retrieved
	assert(rigidBody1.IsValid() && rigidBody2.IsValid() && "Rigidbody(s) invalid. See ResolveDynDynCollision in PhysicsSystem.cpp\n");
}

void ResolveDynStcCollision(glm::vec3* collisionData, ComponentHandle<DynamicCollider2DComponent> collider1, ComponentHandle<StaticCollider2DComponent> collider2)
{
	ComponentHandle<RigidBodyComponent> rigidBody1 = collider1.GetSiblingComponent<RigidBodyComponent>();
	ComponentHandle<TransformComponent> transform1 = collider1.GetSiblingComponent<TransformComponent>();
	// make sure rigidbodies were successfully retrieved
	assert(rigidBody1.IsValid() && transform1.IsValid() && "Rigidbody/Transform invalid. See ResolveDynStcCollision in PhysicsSystem.cpp\n");

	glm::vec3 resolutionVector = *collisionData;

	transform1->Position() += rigidBody1->Velocity() - resolutionVector;

	if (resolutionVector.x)
	{
		rigidBody1->SetVelocity(glm::vec3(0, rigidBody1->Velocity().y, rigidBody1->Velocity().z));
		rigidBody1->SetAcceleration(glm::vec3(0, rigidBody1->Acceleration().y, rigidBody1->Acceleration().z));
	}
	if (resolutionVector.y)
	{
		rigidBody1->SetVelocity(glm::vec3(rigidBody1->Velocity().x, 0, rigidBody1->Velocity().z));
		rigidBody1->SetAcceleration(glm::vec3(rigidBody1->Acceleration().x, 0, rigidBody1->Acceleration().z));
	}
	
}


void UpdateMovementData(ComponentHandle<TransformComponent> transform, ComponentHandle<RigidBodyComponent> rigidBody, glm::vec3 velocity, glm::vec3 acceleration)
{
	transform->Position() += velocity;
	rigidBody->AddVelocity(acceleration + rigidBody->Gravity());
}


void PhysicsSystem::Init()
{

}

void PhysicsSystem::Update(float dt)
{
	// get all rigid bodies
	ComponentMap<RigidBodyComponent> *rigidBodies = GetGameSpace()->GetComponentMap<RigidBodyComponent>();

	for (auto tRigidBodyHandle : *rigidBodies)
	{
		// get the transform from the same gameobject, and leave the loop if it isn't valid
		ComponentHandle<TransformComponent> transform = tRigidBodyHandle.GetSiblingComponent<TransformComponent>();
		ComponentHandle<DynamicCollider2DComponent> dynamicCollider = tRigidBodyHandle.GetSiblingComponent<DynamicCollider2DComponent>();
		
		// if transform and collider are valid, collide it with things
		if (transform.IsValid() && dynamicCollider.IsValid())
		{
			glm::vec3 resolutionVector(0);
			// get all dynamic colliders
			ComponentMap<DynamicCollider2DComponent> *allDynamicColliders = GetGameSpace()->GetComponentMap<DynamicCollider2DComponent>();
			// loop through all dynamic colliders
			for (auto tDynamiColliderHandle : *allDynamicColliders)
			{
				if (dynamicCollider == tDynamiColliderHandle)
				{
					continue;
				}

				resolutionVector = Collision_AABBToAABB(transform, dynamicCollider->ColliderData(), tDynamiColliderHandle.GetSiblingComponent<TransformComponent>(), tDynamiColliderHandle->ColliderData());
			
				// if there was a collision, resolve it
				if (resolutionVector.x || resolutionVector.y)
				{
					ResolveDynDynCollision(&resolutionVector, dynamicCollider, tDynamiColliderHandle);
				}
			}

			// get all static colliders
			ComponentMap<StaticCollider2DComponent> *allStaticColliders = GetGameSpace()->GetComponentMap<StaticCollider2DComponent>();
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
		// if transform is valid and dynamic collider isnt, only update movement !?!? test this!
		else if (transform.IsValid() && !dynamicCollider.IsValid())
		{
			// update position, velocity, and acceleration using stored values
			UpdateMovementData(transform, tRigidBodyHandle, tRigidBodyHandle->Velocity(), tRigidBodyHandle->Acceleration());
		}
	}
}
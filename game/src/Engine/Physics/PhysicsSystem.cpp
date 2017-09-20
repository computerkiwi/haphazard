/*
FILE: PhysicsSystem.cpp
PRIMARY AUTHOR: Brett Schiff

Handles Movement, Colliders, Collision Detection, and Collision Resolution

Copyright © 2017 DigiPen (USA) Corporation.
*/
#include <math.h>

#include "PhysicsSystem.h"
#include "RigidBody.h"
#include "Collider2D.h"


class ColliderBox
{
public:
	ColliderBox(const glm::vec3& center, const glm::vec3& dimensions, const ComponentHandle<RigidBodyComponent> rigidBody, float rotation);

	glm::vec3 m_topRight;
	glm::vec3 m_botLeft;
	float m_rotation;
};

ColliderBox::ColliderBox(const glm::vec3& center, const glm::vec3& dimensions, ComponentHandle<RigidBodyComponent> rigidBody, float rotation)
{
	glm::vec3 centerWithVelocity = center;
	centerWithVelocity += rigidBody->Velocity();

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

int Collision_AABBToAABB(ComponentHandle<TransformComponent>& AABB1Transform, Collider2D& AABB1Collider, ComponentHandle<TransformComponent>& AABB2Transform, Collider2D& AABB2Collider)
{
	ColliderBox Box1(AABB1Transform->Position(), AABB1Collider.GetDimensions(), AABB1Transform.GetSiblingComponent<RigidBodyComponent>(), AABB1Transform->Rotation() + AABB1Collider.GetRotationOffset());
	ColliderBox Box2(AABB2Transform->Position(), AABB2Collider.GetDimensions(), AABB2Transform.GetSiblingComponent<RigidBodyComponent>(), AABB2Transform->Rotation() + AABB2Collider.GetRotationOffset());

	if (Box1.m_topRight.x < Box2.m_botLeft.x)
	{
		return 0;
	}
	if (Box1.m_topRight.y < Box2.m_botLeft.y)
	{
		return 0;
	}
	if (Box1.m_botLeft.x > Box2.m_topRight.x)
	{
		return 0;
	}
	if (Box1.m_botLeft.y > Box2.m_topRight.y)
	{
		return 0;
	}

	return 1;
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
			int TEMPORARYCOLLISIONDATA = 0;
			// get all dynamic colliders
			ComponentMap<DynamicCollider2DComponent> *allDynamicColliders = GetGameSpace()->GetComponentMap<DynamicCollider2DComponent>();
			// loop through all dynamic colliders
			for (auto tDynamiColliderHandle : *allDynamicColliders)
			{
				TEMPORARYCOLLISIONDATA += Collision_AABBToAABB(transform, dynamicCollider->ColliderData(), tDynamiColliderHandle.GetSiblingComponent<TransformComponent>(), tDynamiColliderHandle->ColliderData());
			}

			// get all static colliders
			ComponentMap<StaticCollider2DComponent> *allStaticColliders = GetGameSpace()->GetComponentMap<StaticCollider2DComponent>();
			// loop through all static colliders
			for (auto tStaticColliderHandle : *allStaticColliders)
			{
				TEMPORARYCOLLISIONDATA += Collision_AABBToAABB(transform, dynamicCollider->ColliderData(), tStaticColliderHandle.GetSiblingComponent<TransformComponent>(), tStaticColliderHandle->ColliderData());
			}

			// TEMPORARY: ONLY MOVE IF COLLIDING WITH NOTHING ANYWHERE
			if (TEMPORARYCOLLISIONDATA == 0)
			{
				// update position, velocity, and acceleration using stored values
				transform->Position() += tRigidBodyHandle->Velocity();
				tRigidBodyHandle->AddVelocity(tRigidBodyHandle->Acceleration());
				tRigidBodyHandle->AddVelocity(tRigidBodyHandle->QueuedForce());
			}
		}
		// if transform is valid and dynamic collider isnt, only update movement !?!? test this!
		else if (transform.IsValid() && !dynamicCollider.IsValid())
		{
			// update position, velocity, and acceleration using stored values
			transform->Position() += tRigidBodyHandle->Velocity();
			tRigidBodyHandle->AddVelocity(tRigidBodyHandle->Acceleration());
			tRigidBodyHandle->AddVelocity(tRigidBodyHandle->QueuedForce());
		}
	}
}
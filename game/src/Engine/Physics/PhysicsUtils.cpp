/*
FILE: PhysicsUtils.h
PRIMARY AUTHOR: Brett Schiff

Physics Utilities that are useful outside the physics system

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "PhysicsUtils.h"
#include "Raycast.h" // BoxCorners
#include "PhysicsInternalTools.h" // Collision_PointToBox

bool CollidePointOnLayer(ComponentMap<DynamicCollider2DComponent>* allDynamicColliders, ComponentMap<StaticCollider2DComponent>* allStaticColliders,
						 const glm::vec2& position, collisionLayers layer)
{
	for (auto tDynamiColliderHandle : *allDynamicColliders)
	{
		// the point's collision layer
		CollisionLayer pointLayer(layer);

		// get the object's collider
		Collider2D objectCollider = tDynamiColliderHandle->ColliderData();
		// if the object is on a different layer, skip it
		if (!pointLayer.LayersCollide(objectCollider.GetCollisionLayer()))
		{
			continue;
		}

		// get the object's transform
		ComponentHandle<TransformComponent> objectTransform = tDynamiColliderHandle.GetSiblingComponent<TransformComponent>();
		assert(objectTransform.IsValid() && "Invalid transform from a dynamic collider in CollidePointOnLayer in PhysicsUtils.cpp\n");
		// get the object's position and rotation
		glm::vec2 objectPosition = objectTransform->GetPosition();
		float objectRotation = objectTransform->GetRotation();
		// information about the collider
		glm::vec2 colliderCenter = objectPosition + static_cast<glm::vec2>(objectCollider.GetOffset());
		float colliderRotation = objectRotation + objectCollider.GetRotationOffset();

		// the corners of the collider
		BoxCorners objectCorners(colliderCenter, objectCollider.GetDimensions(), colliderRotation);

		bool result = Collision_PointToBoxQuick(position, objectCorners, objectRotation);

		if (result)
		{
			return true;
		}
	}
	for (auto tStaticColliderHandle : *allStaticColliders)
	{
		// the point's collision layer
		CollisionLayer pointLayer(layer);

		// get the object's collider
		Collider2D objectCollider = tStaticColliderHandle->ColliderData();
		// if the object is on a different layer, skip it
		if (!pointLayer.LayersCollide(objectCollider.GetCollisionLayer()))
		{
			continue;
		}

		// get the object's transform
		ComponentHandle<TransformComponent> objectTransform = tStaticColliderHandle.GetSiblingComponent<TransformComponent>();
		assert(objectTransform.IsValid() && "Invalid transform from a dynamic collider in CollidePointOnLayer in PhysicsUtils.cpp\n");
		// get the object's position and rotation
		glm::vec2 objectPosition = objectTransform->GetPosition();
		float objectRotation = objectTransform->GetRotation();
		// information about the collider
		glm::vec2 colliderCenter = objectPosition + static_cast<glm::vec2>(objectCollider.GetOffset());
		float colliderRotation = objectRotation + objectCollider.GetRotationOffset();

		// the corners of the collider
		BoxCorners objectCorners(colliderCenter, objectCollider.GetDimensions(), colliderRotation);

		bool result = Collision_PointToBoxQuick(position, objectCorners, objectRotation);

		if (result)
		{
			return true;
		}
	}

	return false;
}

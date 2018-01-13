/*
FILE: PhysicsUtils.h
PRIMARY AUTHOR: Brett Schiff

Physics Utilities that are useful outside the physics system

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "PhysicsUtils.h"
#include "Raycast.h" // BoxCorners
#include "PhysicsInternalTools.h" // Collision_PointToBox

glm::vec2 CollidePointOnLayer(ComponentMap<DynamicCollider2DComponent>* allDynamicColliders, ComponentMap<StaticCollider2DComponent>* allStaticColliders,
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

		glm::vec2 result(0,0);

		if (colliderRotation == 0)
		{
			// the corners of the collider
			BoxCorners objectCorners(colliderCenter, objectCollider.GetDimensions(), colliderRotation);

			result = static_cast<glm::vec2>(Collision_AABBToAABB(BoxCorners(position, glm::vec3(0, 0, 0), 0), objectCorners));
		}
		else
		{
			// the corners of the collider
			BoxCorners objectCorners(colliderCenter, objectCollider.GetDimensions(), colliderRotation);

			result = static_cast<glm::vec2>(Collision_SAT(BoxCorners(position, glm::vec2(0.0001, 0.0001), 0), objectCorners));
		}

		if (result.x || result.y)
		{
			return result;
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

		glm::vec2 result(0, 0);

		if (colliderRotation == 0)
		{
			// the corners of the collider
			BoxCorners objectCorners(colliderCenter, objectCollider.GetDimensions(), colliderRotation);

			result = static_cast<glm::vec2>(Collision_AABBToAABB(BoxCorners(position, glm::vec3(0, 0, 0), 0), objectCorners));
		}
		else
		{
			// the corners of the collider
			BoxCorners objectCorners(colliderCenter, objectCollider.GetDimensions(), colliderRotation);

			result = static_cast<glm::vec2>(Collision_SAT(BoxCorners(position, glm::vec2(0.0001, 0.0001), 0), objectCorners));
		}

		if (result.x || result.y)
		{
			return result;
		}
	}

	return glm::vec2(0,0);
}

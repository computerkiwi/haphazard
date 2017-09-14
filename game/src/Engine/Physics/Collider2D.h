/*
FILE: Collider2D.h
PRIMARY AUTHOR: Brett Schiff

Collider2D components and System(Collision Checking)

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "glm/glm.hpp"

class ColliderBox2DComponent
{
public:
	// constructors
	ColliderBox2DComponent(glm::vec3 dimensions, glm::vec3 offset, float rotationOffset = 0) : dimensions_(dimensions), offset_(offset), rotationOffset_(rotationOffset)
	{
	}

	// getters
	glm::vec3 GetDimensions();
	glm::vec3 GetOffset();
	float GetRotationOffset();

	// setters
	void SetDimensions(glm::vec3 newDimensions);
	void SetOffset(glm::vec3 newOffset);
	void SetRotationOffset(float newRotationOffset);

	// methods
		// Passed only one parameter, scales both axes by the same thing
	void ScaleDimensions(float xScale, float yScale = 0);
	void AdjustRotationOffset(float rotationAdjustment);

private:
	glm::vec3 dimensions_;
	glm::vec3 offset_;
	float rotationOffset_;
};
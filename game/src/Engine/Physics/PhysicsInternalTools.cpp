/*
FILE: PhysicsInternalTools.cpp
PRIMARY AUTHOR: Brett Schiff

Useful tools used throughout the physics system that aren't deserving of their own files individually

Copyright 2017 DigiPen (USA) Corporation.
*/
#include "PhysicsInternalTools.h"

CapsuleInformation::CapsuleInformation(glm::vec2 center, glm::vec2 dimensions, float rotation) : 
	                                   m_boxCenter(center),
	                                   m_boxRotation(rotation)
{
	// height of the entire capsule minus 2x the radius of the circle
	float boxHeight = dimensions.y - dimensions.x;
	m_boxDimensions = glm::vec2(dimensions.x, boxHeight);
	
	// the offset from the capsule center of each circle center WITHOUT accounting for rotation
	glm::vec2 circleCenterOffset(0, boxHeight / 2);
	// if the capsule is rotated, account for it
	if (rotation != 0)
	{
		// get the scalar of the total each side will be
		float xScalar = sin(DegreesToRadians(rotation));
		float yScalar = cos(DegreesToRadians(rotation));

		circleCenterOffset.x = circleCenterOffset.y * xScalar;
		circleCenterOffset.y = circleCenterOffset.y * yScalar;
	}

	// get the center of each circle by adding the offset to each
	m_topCircleCenter = center + circleCenterOffset;
	m_botCircleCenter = center - circleCenterOffset;

	// get the circle's radius
	m_circleRadius = dimensions.x / 2;
}

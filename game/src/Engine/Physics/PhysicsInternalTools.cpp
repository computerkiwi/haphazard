/*
FILE: PhysicsInternalTools.cpp
PRIMARY AUTHOR: Brett Schiff

Useful tools used throughout the physics system that aren't deserving of their own files individually

Copyright 2017 DigiPen (USA) Corporation.
*/
#include <algorithm> // min, max

#include "PhysicsInternalTools.h"
#include "../../graphics/DebugGraphic.h"


#define DEGREES_PER_RADIAN 57.2957795131f


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

MinMax BoxCorners::ProjectOntoAxis(glm::vec2 axis) const
{
	float firstDot = glm::dot(axis, m_corners[0]);
	// we only care about the min and max values
	MinMax minMax = { firstDot, firstDot };

	glm::vec2 startpos = m_corners[0];
	glm::vec2 direction = glm::normalize(axis);

	DrawNormalizedRay(startpos, direction, 1);

	// project each of the four points onto the axis and record the extrema
	for (int i = 1; i < 4; ++i)
	{
		float dotP = glm::dot(axis, m_corners[i]);

		DrawSmallCircleAtPosition(startpos + (dotP * direction));

		if (dotP < minMax.min)
		{
			minMax.min = dotP;
		}
		else if (dotP > minMax.max)
		{
			minMax.max = dotP;
		}
	}

	return minMax;
}

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
		float xScalar = sin(DegreesToRadians(-rotation));
		float yScalar = cos(DegreesToRadians(-rotation));

		circleCenterOffset.x = circleCenterOffset.y * xScalar;
		circleCenterOffset.y = circleCenterOffset.y * yScalar;
	}

	// get the center of each circle by adding the offset to each
	m_topCircleCenter = center + circleCenterOffset;
	m_botCircleCenter = center - circleCenterOffset;

	// get the circle's radius
	m_circleRadius = dimensions.x / 2;
}

bool MinMax::Intersects(const MinMax& other)
{
	return min < other.max && max > other.min;
}

// find the overlap between two sets
float MinMax::Overlap(const MinMax& other)
{
	float magnitude = std::min(max, other.max) - std::max(min, other.min);

	if (max > other.max)
	{
		return -magnitude;
	}
	else
	{
		return magnitude;
	}
}

// returns the float with the bigger absolute magnitude
float maxAbs(float lhs, float rhs)
{
	if (abs(lhs) > abs(rhs))
	{
		return lhs;
	}
	else
	{
		return rhs;
	}
}

float vectorToDirection(glm::vec2& dirVector)
{
	return atan2(dirVector.y, dirVector.x);
}

float DegreesToRadians(float angleInDegrees)
{
	return angleInDegrees / DEGREES_PER_RADIAN;
}

float RadiansToDegrees(float angleInRadians)
{
	return angleInRadians * DEGREES_PER_RADIAN;
}

bool CirclesCollide(glm::vec2 center1, float radius1, glm::vec2 center2, float radius2)
{
	glm::vec2 distanceVec = center1 - center2;
	float distanceSquared = (distanceVec.x * distanceVec.x) + (distanceVec.y * distanceVec.y);

	return distanceSquared < ((radius1 + radius2) * (radius1 + radius2));
}

void DrawSmallBoxAtPosition(glm::vec2 position)
{
	DebugGraphic::DrawSquare(position, glm::vec2(.1f, .1f), 0, glm::vec4(1, 0, 1, 1));
}

void DrawSmallCircleAtPosition(glm::vec2 position)
{
	DebugGraphic::DrawCircle(position, 0.05f, glm::vec4(1, 0, 1, 1));
}

float CrossP(glm::vec2 vec1, glm::vec2 vec2)
{
	return (vec1.x * vec2.y) - (vec1.y * vec2.x);
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

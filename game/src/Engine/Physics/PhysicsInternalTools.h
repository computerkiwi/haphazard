/*
FILE: Collider2D.h
PRIMARY AUTHOR: Brett Schiff

Useful tools used throughout the physics system that aren't deserving of their own files individually

Copyright 2017 DigiPen (USA) Corporation.
*/
#pragma once
#include "Raycast.h" // glm::vec2, BoxCorners

bool Collision_PointToBoxQuick(const glm::vec2& point, const BoxCorners& box, float boxRotation);

void DrawSmallBoxAtPosition(glm::vec2 position);

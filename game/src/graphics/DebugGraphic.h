/*
FILE: DebugGraphic.h
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "glm\glm.hpp"
#include <vector>

namespace DebugGraphic
{
	// Draws shape for one update. Positition and scale are both relative to world space
	void DrawSquare(glm::vec2 pos, glm::vec2 scale = glm::vec2(1, 1), float rotationRadians = 0, glm::vec4 color = glm::vec4(1, 0, 0, 1));

	void DrawCircle(glm::vec2 pos, float radius = 1, glm::vec4 color = glm::vec4(1, 0, 0, 1));

	// Draw all debug shapes and CLEARS ALL CURRENT DEBUG SHAPES
	void DrawAll();

	// Sets thiccness of all debug shapes
	void SetLineWidth(float size);
};

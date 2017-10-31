/*
FILE: DebugGraphic.h
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "glm\glm.hpp"
#include <vector>

class DebugGraphic
{
public:
	// Draws shape for one update. Positition and scale are both relative to world space
	static void DrawShape(glm::vec2 pos, glm::vec2 scale = glm::vec2(1, 1), float rotationRadians = 0, glm::vec4 color = glm::vec4(1, 0, 0, 1));

	// Draw all debug shapes and CLEARS ALL CURRENT DEBUG SHAPES
	static void DrawAll();

	// Sets thiccness of all debug shapes
	static void SetLineWidth(float size);
private:
	DebugGraphic() = delete;

	// Vertex instancing data
	static std::vector<float> m_ShapeData;
	static unsigned int m_VAO;
	static unsigned int m_VBO;
};
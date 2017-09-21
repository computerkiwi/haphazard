#pragma once

#include "glm\glm.hpp"
#include <vector>

class DebugGraphic
{
public:

	static void DrawShape(glm::vec2 pos, glm::vec2 scale = glm::vec2(1, 1), float rotationRadians = 0, glm::vec4 color = glm::vec4(1, 0, 0, 1));

	static void DrawAll();
private:

	static std::vector<float> shapeData;
	static unsigned int VAO;
	static unsigned int VBO;
};
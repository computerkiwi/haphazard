#pragma once

#include "glm\glm.hpp"
#include <list>

class DebugGraphic
{
public:
	DebugGraphic(glm::vec2 pos, int vertices = 4);
	~DebugGraphic();

	static void DrawAll();
private:

	static std::list<DebugGraphic*> shapes;
	static int amount;
	static unsigned int VAO;
	static unsigned int VBO;

	glm::vec2 m_position;
	int m_vertices;
	int m_index;
};
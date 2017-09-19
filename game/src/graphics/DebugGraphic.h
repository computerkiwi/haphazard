#pragma once

#include "glm\glm.hpp"
#include <list>

class DebugGraphic
{
public:
	DebugGraphic(glm::vec2 pos, glm::vec2 scale = glm::vec2(1,1), int vertices = 4);
	~DebugGraphic();

	void SetScale(glm::vec2 s) { m_scale = s; }
	void SetScale(float x, float y) { m_scale.x = x; m_scale.y = y; }

	void SetPosition(glm::vec2 p) { m_position = p; }
	void SetPosition(float x, float y) { m_position.x = x; m_position.y = y; }

	glm::vec2 GetScale() { return m_scale; }
	glm::vec2 GetPosition() { return m_position; }

	static void DrawAll();
private:

	static std::list<DebugGraphic*> shapes;
	static int amount;
	static unsigned int VAO;
	static unsigned int VBO;

	glm::vec2 m_position;
	glm::vec2 m_scale;
	int m_vertices;
	int m_index;
};
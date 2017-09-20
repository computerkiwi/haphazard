#pragma once

#include "glm\glm.hpp"
#include <list>

class DebugGraphic
{
public:
	DebugGraphic(glm::vec2 pos, glm::vec2 scale = glm::vec2(1, 1), float rotation = 0, glm::vec4 color = glm::vec4(1,0,0,1) );
	~DebugGraphic();

	void SetScale(glm::vec2 s) { m_scale = s; }
	void SetScale(float x, float y) { m_scale.x = x; m_scale.y = y; }

	void SetPosition(glm::vec2 p) { m_position = p; }
	void SetPosition(float x, float y) { m_position.x = x; m_position.y = y; }

	void SetRotation(float r) { m_rotation; }

	void SetColor(glm::vec4 c) { m_color = c; }
	void SetColor(float r, float g, float b, float a) 
		{ m_color.x = r; m_color.y = g; m_color.z = b; m_color.w = a; }

	glm::vec2 GetScale() { return m_scale; }
	glm::vec2 GetPosition() { return m_position; }
	float GetRotation() { return m_rotation; }
	glm::vec4 GetColor() { return m_color; }

	static void DrawAll();
private:

	static std::list<DebugGraphic*> shapes;
	static int amount;
	static unsigned int VAO;
	static unsigned int VBO;

	glm::vec2 m_position;
	glm::vec2 m_scale;
	float m_rotation;
	glm::vec4 m_color;
	int m_index;
};
/*
FILE: DebugGraphic.cpp
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "DebugGraphic.h"
#include "Shaders.h"

std::vector<float> DebugGraphic::m_ShapeData;
unsigned int DebugGraphic::m_VAO;
unsigned int DebugGraphic::m_VBO;

void DebugGraphic::DrawShape(glm::vec2 pos, glm::vec2 scale, float rotation, glm::vec4 color)
{
	m_ShapeData.push_back(pos.x);
	m_ShapeData.push_back(pos.y);
	m_ShapeData.push_back(scale.x);
	m_ShapeData.push_back(scale.y);
	m_ShapeData.push_back(rotation);
	m_ShapeData.push_back(color.x);
	m_ShapeData.push_back(color.y);
	m_ShapeData.push_back(color.z);
	m_ShapeData.push_back(color.w);
}

void DebugGraphic::DebugGraphic::DrawAll()
{
	if (!m_VBO)
	{
		glGenBuffers(1, &m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	}

	if (!m_VAO)
	{
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);
		Shaders::debugShader->ApplyAttributes();
	}

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	Shaders::debugShader->Use();

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_ShapeData.size(), m_ShapeData.data(), GL_STATIC_DRAW);
	glDrawArraysInstanced(GL_POINTS, 0, 9, (GLsizei)m_ShapeData.size() / 9);

	m_ShapeData.clear();
}

void DebugGraphic::SetLineWidth(float size)
{
	glLineWidth(size);
}
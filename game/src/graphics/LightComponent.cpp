/*
FILE: LightComponent.cpp
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "LightComponent.h"
#include "Shaders.h"

GLuint LightComponent::m_instanceVBO = 0;
GLuint LightComponent::m_vertexVBO = 0;
GLuint LightComponent::m_VAO = 0;

void LightComponent::SetUpBuffers()
{
	glGenBuffers(1, &m_instanceVBO);
	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
	Shaders::lightShader->ApplyAttributes(1,4);

	float verts[]  // vertices that fill up entire screan
	{
		-1, 1,	-1,-1,	 1,-1,
		 1,-1,	 1, 1,	-1, 1,
	};
	glGenBuffers(1, &m_vertexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexVBO);
	Shaders::lightShader->ApplyAttributes(0, 1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
}

LightComponent::LightComponent(float range, glm::vec4 color)
	: m_range{ range }, m_color{ color } 
{
	if (!m_instanceVBO)
	{
		SetUpBuffers();
	}
}

void LightComponent::SetRenderData(glm::vec2 pos, std::vector<float>* data)
{
	if (!m_enabled)
		return;

	data->push_back(pos.x);
	data->push_back(pos.y);

	data->push_back(m_color.x);
	data->push_back(m_color.y);
	data->push_back(m_color.z);
	data->push_back(m_color.w);

	data->push_back(m_range);
}

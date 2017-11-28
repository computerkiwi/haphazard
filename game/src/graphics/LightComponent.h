/*
FILE: LightComponent.h
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once
#include "glm\glm.hpp"
#include "meta/meta.h"
#include "GL\glew.h"
#include <vector>

class LightComponent
{
	friend class RenderSystem;
public:
	LightComponent(float range = 1.0f, glm::vec4 color = glm::vec4(1, 1, 1, 1));

	void SetRange(float range)      { m_range = range;  }
	void SetColor(glm::vec4 color)  { m_color = color;  }

	void Enable()    { m_enabled = true; }
	void Disable()   { m_enabled = true; }

	glm::vec4 GetColor() const { return m_color;   }
	float GetRange() const     { return m_range;   }
	bool IsEnabled() const     { return m_enabled; }


private:
	glm::vec4 m_color;
	float m_range;
	bool m_enabled = true;

private: // Instancing Buffers
	static GLuint m_vertexVBO;
	static GLuint m_instanceVBO;
	static GLuint m_VAO;

	void SetRenderData(glm::vec2 pos, std::vector<float>* data);

	static const int DataSize() { return 2 + 4 + 1; } // Position + Color + Range
	static void BindInstanceVBO() { glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO); }
	static void BindVertexVBO() { glBindBuffer(GL_ARRAY_BUFFER, m_vertexVBO); }
	static void BindVAO() { glBindVertexArray(m_VAO); }
	static void SetUpBuffers();


	META_REGISTER(LightComponent)
	{
		META_DefineType(glm::vec4); // HACK

		META_DefineType(LightComponent);
	}
};
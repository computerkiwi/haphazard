#include "DebugGraphic.h"
#include "Shaders.h"

std::list<DebugGraphic*> DebugGraphic::shapes;
int DebugGraphic::amount;
unsigned int DebugGraphic::VAO;
unsigned int DebugGraphic::VBO;

DebugGraphic::DebugGraphic(glm::vec2 pos, glm::vec2 scale, float rotation, glm::vec4 color)
	: m_position(pos), m_scale(scale), m_rotation(rotation), m_color(color), m_index(amount)
{
	if (!VBO)
	{
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
	}

	if (!VAO)
	{
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		Graphics::Shaders::debugShader->ApplyAttributes();
	}


	shapes.push_back(this);
	amount++;
}

DebugGraphic::~DebugGraphic()
{
	shapes.remove(this);
	amount--;
}

void DebugGraphic::DebugGraphic::DrawAll()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	Graphics::Shaders::debugShader->Use();

	float *data = new float[amount * 9]; // 9 floats per shape
	int i = 0;

	for (auto shape : shapes)
	{
		data[i++] = shape->m_position.x;
		data[i++] = shape->m_position.y;

		data[i++] = shape->m_scale.x;
		data[i++] = shape->m_scale.y;

		data[i++] = shape->m_rotation;

		data[i++] = shape->m_color.x;
		data[i++] = shape->m_color.y;
		data[i++] = shape->m_color.z;
		data[i++] = shape->m_color.w;
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * i, data, GL_STATIC_DRAW);

	glDrawArraysInstanced(GL_POINTS, 0, amount, amount);

	delete[] data;
}

#include "DebugGraphic.h"
#include "Shaders.h"

std::vector<float> DebugGraphic::shapeData;
unsigned int DebugGraphic::VAO;
unsigned int DebugGraphic::VBO;

void DebugGraphic::DrawShape(glm::vec2 pos, glm::vec2 scale, float rotation, glm::vec4 color)
{
	shapeData.push_back(pos.x);
	shapeData.push_back(pos.y);
	shapeData.push_back(scale.x);
	shapeData.push_back(scale.y);
	shapeData.push_back(rotation);
	shapeData.push_back(color.x);
	shapeData.push_back(color.y);
	shapeData.push_back(color.z);
	shapeData.push_back(color.w);
}

void DebugGraphic::DebugGraphic::DrawAll()
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
		Shaders::debugShader->ApplyAttributes();
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	Shaders::debugShader->Use();

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shapeData.size(), shapeData.data(), GL_STATIC_DRAW);
	glDrawArraysInstanced(GL_POINTS, 0, 9, (GLsizei)shapeData.size() / 9);

	shapeData.clear();
}

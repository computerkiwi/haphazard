/*
FILE: DebugGraphic.cpp
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "DebugGraphic.h"
#include "Shaders.h"
#include "VertexObjects.h"

static std::vector<float> shapeData;
static BufferObject* dataBuffer = nullptr;
static VertexAttributeBindings* attribBindings = nullptr;


void DebugGraphic::DrawSquare(glm::vec2 pos, glm::vec2 scale, float rotation, glm::vec4 color)
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
	shapeData.push_back(0.0f); // Square
}

void DebugGraphic::DrawCircle(glm::vec2 pos, float radius, glm::vec4 color)
{
	shapeData.push_back(pos.x);
	shapeData.push_back(pos.y);
	shapeData.push_back(radius); // scale.x
	shapeData.push_back(0); // scale.y
	shapeData.push_back(0); // rotation
	shapeData.push_back(color.x);
	shapeData.push_back(color.y);
	shapeData.push_back(color.z);
	shapeData.push_back(color.w);
	shapeData.push_back(1.0f); // Circle
}

void DebugGraphic::DrawAll()
{
	if (dataBuffer == nullptr)
	{
		dataBuffer = new BufferObject();
		attribBindings = new VertexAttributeBindings();

		attribBindings->BindAttributesToBuffer(Shaders::debugShader, *dataBuffer);
	}

	Shaders::debugShader->Use();
	attribBindings->Use();
	dataBuffer->SetData(sizeof(float), static_cast<int>(shapeData.size()), shapeData.data());

	glDrawArraysInstanced(GL_POINTS, 0, 10, (GLsizei)shapeData.size() / 10);

	shapeData.clear();
}

void DebugGraphic::SetLineWidth(float size)
{
	glLineWidth(size);
}
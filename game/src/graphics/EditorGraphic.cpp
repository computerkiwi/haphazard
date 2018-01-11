/*
FILE: EditorGraphic.cpp
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "EditorGraphic.h"
#include "Shaders.h"
#include "Texture.h"
#include "VertexObjects.h"

//static GLuint VAO, VBO;
static BufferObject* dataBuffer = nullptr;
static VertexAttributeBindings* attribBindings = nullptr;
static bool generated = false;

void GenerateEditorSpriteBuffers()
{
	Shaders::editorSpriteShader->Use();

	dataBuffer = new BufferObject();
	attribBindings = new VertexAttributeBindings();

	attribBindings->BindAttributesToBuffer(Shaders::editorSpriteShader, *dataBuffer);

	float data[] = 
	{
		-0.5f,  0.5f,  0, 0,
		-0.5f, -0.5f,  0, 1,
		 0.5f, -0.5f,  1, 1,

		 0.5f, -0.5f,  1, 1,
		 0.5f,  0.5f,  1, 0,
		-0.5f,  0.5f,  0, 0
	};
	dataBuffer->SetData(1, sizeof(data), data);

	generated = true;
}

void DrawEditorSprite(Resource* res, glm::vec2 pos, glm::vec2 scale, float rotationRadians)
{
	if (!generated)
		GenerateEditorSpriteBuffers();

	attribBindings->Use();

	Shaders::editorSpriteShader->Use();
	Shaders::editorSpriteShader->SetVariable("Position",pos);
	Shaders::editorSpriteShader->SetVariable("Scale", scale);
	Shaders::editorSpriteShader->SetVariable("Rotation", rotationRadians);

	Shaders::editorSpriteShader->SetVariable("TexLayer", reinterpret_cast<Texture*>(res->Data())->GetLayer());
	Shaders::editorSpriteShader->SetVariable("TexBox", reinterpret_cast<Texture*>(res->Data())->GetBounds());
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

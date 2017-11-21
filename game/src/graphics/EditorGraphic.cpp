/*
FILE: EditorGraphic.cpp
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "EditorGraphic.h"
#include "Shaders.h"
#include "Texture.h"

static GLuint VAO, VBO;
static bool generated = false;

void GenerateEditorSpriteBuffers()
{
	Shaders::editorSpriteShader->Use();

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	Shaders::editorSpriteShader->ApplyAttributes();

	float data[] = 
	{
		-0.5f,  0.5f,  0, 0,
		-0.5f, -0.5f,  0, 1,
		 0.5f, -0.5f,  1, 1,

		 0.5f, -0.5f,  1, 1,
		 0.5f,  0.5f,  1, 0,
		-0.5f,  0.5f,  0, 0
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	generated = true;
}

void DrawEditorSprite(Resource* res, glm::vec2 pos, glm::vec2 scale, float rotationRadians)
{
	if (!generated)
		GenerateEditorSpriteBuffers();

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	Shaders::editorSpriteShader->Use();
	Shaders::editorSpriteShader->SetVariable("Position",pos);
	Shaders::editorSpriteShader->SetVariable("Scale", scale);
	Shaders::editorSpriteShader->SetVariable("Rotation", rotationRadians);

	Shaders::editorSpriteShader->SetVariable("TexLayer", reinterpret_cast<Texture*>(res->Data())->GetLayer());
	Shaders::editorSpriteShader->SetVariable("TexBox", reinterpret_cast<Texture*>(res->Data())->GetBounds());
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

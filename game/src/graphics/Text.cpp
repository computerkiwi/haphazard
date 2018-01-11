/*
FILE: Text.cpp
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "Engine\Engine.h"
#include "Text.h"

#include "Util\Logging.h"
#include "GL\glew.h"
#include "Texture.h"
#include "SOIL\SOIL.h"
#include "Shaders.h"

///
// Font
///

Font* Fonts::arial;

void Font::InitFonts()
{
	Fonts::arial = new Font("font.png", 24, 32, 16, 6);
}

Font::Font(const char* path, int charWidth, int charHeight, int numCharsX, int numCharsY)
{
	m_Texture = new AnimatedTexture(path, charWidth, charHeight, numCharsX, numCharsY, numCharsX * numCharsY);
}


///
// Text
///
BufferObject* TextComponent::m_VertexBuffer = nullptr;

void TextComponent::SetVertexBuffer()
{
	//glBindBuffer(GL_ARRAY_BUFFER, m_VertexVBO);

	float data[] =
	{
		/*Position*/       /*Tex coords*/
		-0.5f,  0.5f, 0,  0, 1, // Top Left
		-0.5f, -0.5f, 0,  0, 0, // Bot Left
		0.5f,  -0.5f, 0,  1, 0,  // Bot Right

		0.5f,  -0.5f, 0,  1, 0, // Bot Right
		0.5f,   0.5f, 0,  1, 1, // Top Right
		-0.5f,  0.5f, 0,  0, 1  // Top Left
	};
	//glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	m_VertexBuffer->SetData(1, sizeof(data), data);
}

TextComponent::TextComponent(std::string string, Font* font, glm::vec4 color)
	: m_Font { font }, m_Color{ color }
{	
	if (m_VertexBuffer == nullptr)
	{
		m_VertexBuffer = new BufferObject();
		SetVertexBuffer();
	}

	m_AttribBindings.BindAttributesToBuffer(Shaders::textShader, *m_VertexBuffer, 0, 2);
	m_AttribBindings.BindAttributesToBuffer(Shaders::textShader, m_CharBuffer, 2, 5);

	/*
	if (!m_VertexVBO)
		glGenBuffers(1, &m_VertexVBO);

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VertexVBO);
	Shaders::textShader->ApplyAttributes(0, 2);

	glGenBuffers(1, &m_CharVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_CharVBO);
	Shaders::textShader->ApplyAttributes(2, 5);
	*/

	CompileText(string);
}

void TextComponent::SetText(std::string string, Font* font, glm::vec4* color)
{
	if (font)
		m_Font = font;
	if (color)
		m_Color = *color;

	CompileText(string);
}

void TextComponent::CompileText(std::string string)
{
	m_CharData.clear();
	
	glm::vec2 p = glm::vec2(0, 0);
	for (char c : string)
	{
		// Char Position
		m_CharData.push_back(p.x);
		m_CharData.push_back(p.y);

		p.x += m_Font->GetFontSpacing();
		if (c == '\n')
			p.y += m_Font->GetFontSpacing();

		// TexBox
		glm::vec4 pos = m_Font->CharBox(c);
		m_CharData.push_back(pos.x);
		m_CharData.push_back(pos.w);
		m_CharData.push_back(pos.z);
		m_CharData.push_back(pos.y);

		// Color
		m_CharData.push_back(m_Color.x);
		m_CharData.push_back(m_Color.y);
		m_CharData.push_back(m_Color.z);
		m_CharData.push_back(m_Color.w);
	}
}

void TextComponent::Draw(glm::mat4& matrix)
{
	Shaders::textShader->Use();
	//glBindVertexArray(m_VAO);
	m_AttribBindings.Use();

	//glBindBuffer(GL_ARRAY_BUFFER, m_CharVBO);
	
	Shaders::textShader->SetVariable("model", matrix);
	Shaders::textShader->SetVariable("font", m_Font->GetTextureLayer());

	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_CharData.size(), m_CharData.data(), GL_STATIC_DRAW);
	m_CharBuffer.SetData(sizeof(float), m_CharData.size(), m_CharData.data());

	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, (GLsizei)(m_CharData.size()/10) ); // 10 = num of attrib floats
}

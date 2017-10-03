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
	Fonts::arial = new Font("arial.png", 48, 10);
}

Font::Font(const char* path, int fontSize, int numCharsX)
	: m_CharsWide { numCharsX }
{
	m_Texture = new AnimatedTexture(path, fontSize, fontSize, numCharsX, numCharsX);
	
	//m_CharSize = glm::vec2(m_Width / (float)fontSize, m_Height / (float)fontSize);
	/*
	if (!m_VBO)
	{
		glGenBuffers(1, &m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	}
	*/
}


///
// Text
///

GLuint Text::m_CharVBO = 0;

Text::Text(std::string string, Font* font, glm::vec4 color)
	: m_String { string }, m_Font { font }, m_Color{ color }
{
	Shaders::textShader->Use();
	if (!m_VAO)
	{
		glGenVertexArrays(1, &m_VAO);
	}
	glBindVertexArray(m_VAO);

	if (!m_VertexVBO)
	{
		glGenBuffers(1, &m_VertexVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexVBO);
	}

	for (char c : m_String)
	{
		glm::vec2 pos = font->CharLocation(c);
		m_CharData.push_back(pos.x);
		m_CharData.push_back(pos.y);
		m_CharData.push_back(color.x);
		m_CharData.push_back(color.y);
		m_CharData.push_back(color.z);
		m_CharData.push_back(color.w);
	}
}

void Text::GetDrawData(float* data)
{
}
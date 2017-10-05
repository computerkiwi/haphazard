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
	Fonts::arial = new Font("ConsolasFont.png", 24, 20, 5);
}

Font::Font(const char* path, int fontSize, int numCharsX, int numCharsY)
{
	m_Texture = new AnimatedTexture(path, fontSize, fontSize, numCharsX, numCharsY);
	m_FontSize = m_Texture->GetSpriteSize();
}


///
// Text
///
GLuint Text::m_VertexVBO = 0;

void Text::SetVertexBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexVBO);
	float data[20] =
	{
		/*Position*/       /*Tex coords*/
		-0.5f, -0.5f,  0,  0, 0,
		 0.5f,  -0.5f, 0,  1, 0,
		 0.5f,   0.5f, 0,  1, 1,
		-0.5f,  0.5f,  0,  0, 1,
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
}

Text::Text(std::string string, Font* font, glm::vec4 color)
	: m_String { string }, m_Font { font }, m_Color{ color }
{	
	if (!m_VertexVBO)
		glGenBuffers(1, &m_VertexVBO);

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VertexVBO);
	Shaders::textShader->ApplyAttributes(0, 2);

	glGenBuffers(1, &m_FontVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_FontVBO);
	Shaders::textShader->ApplyAttributes(2, 3);

	glGenBuffers(1, &m_CharVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_CharVBO);
	Shaders::textShader->ApplyAttributes(3, 10);

	SetVertexBuffer();

	CompileText();
}

void Text::SetText(std::string string, Font* font, glm::vec4* color)
{
	m_String = string;
	if (font)
		m_Font = font;
	if (color)
		m_Color = *color;

	CompileText();
}

void Text::CompileText()
{
	m_CharData.clear();
	m_FontData.clear();

	glm::vec2 p = glm::vec2(0, 0);
	for (char c : m_String)
	{
		// Char Position
		p.x += m_Font->GetFontSize().x;
		if (c == '\n')
			p.y += m_Font->GetFontSize().y;

		m_CharData.push_back(p.x);
		m_CharData.push_back(p.y);

		// TexBox
		glm::vec2 pos = m_Font->CharLocation(c);
		m_CharData.push_back(pos.x);
		m_CharData.push_back(pos.y);

		// Color
		m_CharData.push_back(m_Color.x);
		m_CharData.push_back(m_Color.y);
		m_CharData.push_back(m_Color.z);
		m_CharData.push_back(m_Color.w);

		// Font
		m_FontData.push_back(m_Font->GetTextureLayer());
	}
}

void Text::Draw(glm::mat4& matrix)
{
	Shaders::textShader->Use();
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_FontVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(int) * m_FontData.size(), m_FontData.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_CharVBO);
	
	std::vector<float> data = m_CharData;
	for (int i = 0; i < 4 * 4; i++)
		data.push_back(matrix[i / 4][i % 4]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), data.data(), GL_STATIC_DRAW);

	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, (GLsizei)m_String.length());
}

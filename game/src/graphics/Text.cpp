#include "Text.h"

#include "Util\Logging.h"
#include "GL\glew.h"
#include "Texture.h"
#include "SOIL\SOIL.h"

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
	unsigned char* image = SOIL_load_image(path, &m_Width, &m_Height, 0, SOIL_LOAD_RGBA);

	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_INT, image);

	SOIL_free_image_data(image);

	m_CharSize = glm::vec2(m_Width / (float)fontSize, m_Height / (float)fontSize);
}

void Font::BindFont()
{
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
}


///
// Text
///

Text::Text(std::string string, Font* font, glm::vec4 color)
	: m_String { string }, m_Font { font }, m_Color{ color }
{

}
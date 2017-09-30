#include "glm\glm.hpp"
#include <string>

typedef unsigned int GLuint;

class Font
{
public:
	void InitFonts();
	void BindFont();
	glm::vec2 CharLocation(char c)
	{
		c -= '!'; // First character = !
		return glm::vec2(c % m_CharsWide * m_CharSize.x, c / m_CharsWide * m_CharSize.y);
	}
private:
	Font(const char* path, int fontSize, int numCharsX);

	GLuint m_TextureID;
	int m_Width, m_Height;
	glm::vec2 m_CharSize;
	int m_CharsWide;
};

namespace Fonts
{
	extern Font* arial;
}

//   "Important things are `this` color. Pickups are ``this`` color. ", WHITE, RED, PURPLE

class Text
{
public:
	Text(std::string string, Font* font, glm::vec4 colors);

private:

	std::string m_String;
	Font* m_Font;
	glm::vec4 m_Color;
};
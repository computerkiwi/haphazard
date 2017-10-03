#include "glm\glm.hpp"
#include <string>
#include <vector>

typedef unsigned int GLuint;

class Font
{
public:
	void InitFonts();
	glm::vec2 CharLocation(char c)
	{
		c -= '!'; // First character = !
		m_Texture->GetFrameCoords(c);
	}
private:
	Font(const char* path, int fontSize, int numCharsX);

	AnimatedTexture* m_Texture;
	int m_Width, m_Height;
	glm::vec2 m_CharSize;
	int m_CharsWide;

	GLuint m_VBO;
};

namespace Fonts
{
	extern Font* arial;
}

class Text
{
public:
	Text(std::string string, Font* font, glm::vec4 colors);
	void GetDrawData(float* data);

private:
	static GLuint m_CharVBO;
	static float m_VertData[8];

	std::vector<float> m_CharData;

	std::string m_String;
	Font* m_Font;
	glm::vec4 m_Color;

	static GLuint m_VAO;
	static GLuint m_VertexVBO;
};

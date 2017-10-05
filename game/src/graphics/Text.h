#include "glm\glm.hpp"
#include "Texture.h"
#include <string>
#include <vector>

typedef unsigned int GLuint;

class Font
{
public:
	static void InitFonts();

	glm::vec2 GetFontSize() { return m_FontSize; }
	GLuint GetTextureLayer() { return m_Texture->GetID(); }
	glm::vec2 CharLocation(char c)
	{
		c -= '!'-1; // First character = one before !
		return m_Texture->GetFrameCoords(c);
	}
private:
	Font(const char* path, int fontSize, int numCharsX, int numCharsY);
	
	AnimatedTexture* m_Texture;
	glm::vec2 m_FontSize;
};

namespace Fonts
{
	extern Font* arial;
}

class Text
{
public:
	Text(std::string string, Font* font, glm::vec4 color);
	void SetText(std::string string, Font* font = nullptr, glm::vec4* color = nullptr);
	void Draw(glm::mat4& matrix);

private:
	static void SetVertexBuffer();

	void CompileText();

	std::vector<float> m_CharData;
	std::vector<int> m_FontData;
	std::string m_String;
	Font* m_Font;
	glm::vec4 m_Color;

	// Buffers
	GLuint m_VAO;

	static GLuint m_VertexVBO;
	GLuint m_CharVBO = 0;
	GLuint m_FontVBO = 0;
};

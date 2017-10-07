#include "glm\glm.hpp"
#include "Texture.h"
#include <string>
#include <vector>

typedef unsigned int GLuint;

class Font
{
public:
	static void InitFonts();

	float GetFontSpacing() { return 1; }
	GLuint GetTextureLayer() { return m_Texture->GetID(); }
	glm::vec4 CharBox(char c)
	{
		c -= '!'-1; // First character = one before !
		return glm::vec4(m_Texture->GetFrameCoords(c), m_Texture->GetFrameCoords(c) + m_Texture->GetSpriteSize());
	}
private:
	Font(const char* path, int charWidth, int charHeight, int numCharsX, int numCharsY);
	
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

	void CompileText(std::string string);

	std::vector<float> m_CharData;
	Font* m_Font;
	glm::vec4 m_Color;

	// Buffers
	static GLuint m_VertexVBO;
	GLuint m_VAO;
	GLuint m_CharVBO = 0;
};

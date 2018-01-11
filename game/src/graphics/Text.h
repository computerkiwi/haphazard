/*
FILE: Text.h
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "Universal.h"
#include "glm\glm.hpp"
#include "Texture.h"
#include <string>
#include <vector>
#include "VertexObjects.h"

typedef unsigned int GLuint;

class Font;

namespace Fonts
{
	extern Font* arial;
}



class Font
{
public:
	static void InitFonts();

	// Get relative space between characters
	float GetFontSpacing() { return m_FontSpacing; }

	// Set relative space between characters
	void SetFontSpacing(float space) { m_FontSpacing = space; }

	// Return font texture
	GLuint GetTextureLayer() { return m_Texture->GetLayer(); }

	// Get texture box around character c 
	glm::vec4 CharBox(char c)
	{
		c -= '!'-1; // First character on spritesheet is one character before '!'
		return m_Texture->GetBounds(c);
	}

private:
	Font(const char* path, int charWidth, int charHeight, int numCharsX, int numCharsY);
	
	AnimatedTexture* m_Texture;
	float m_FontSpacing = 1;
};

class TextComponent
{
public:
	TextComponent(std::string string = std::string(""), Font* font = Fonts::arial, glm::vec4 color = glm::vec4(1,1,1,1));
	void SetText(std::string string, Font* font = nullptr, glm::vec4* color = nullptr);
	void Draw(glm::mat4& matrix);

private:
	static void SetVertexBuffer();

	void CompileText(std::string string);

	std::vector<float> m_CharData; // String instance data
	Font* m_Font;
	glm::vec4 m_Color;

	// Buffers
	//static GLuint m_VertexVBO;
	//GLuint m_VAO;
	//GLuint m_CharVBO = 0;
	static BufferObject* m_VertexBuffer;

	VertexAttributeBindings m_AttribBindings;
	BufferObject m_CharBuffer;

	META_REGISTER(TextComponent)
	{
	}
};

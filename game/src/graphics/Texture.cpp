#include "Mesh.h"
#include "SOIL\SOIL.h"

///
// Texture
///

GLuint Texture::m_TextureArray = 0;
int Texture::m_layers = 0;

void Texture::GenerateTextureArray()
{
	glGenTextures(1, &m_TextureArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_TextureArray);

	// Warning: If larger or more textures are needed, the size cannot exceed GL_MAX_3D_TEXTURE_SIZE 
	// 3 mipmap levels, max layers is 128
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 3, GL_RGBA8, Texture::MAX_WIDTH, Texture::MAX_HEIGHT, 128);
	GL_MAX_3D_TEXTURE_SIZE;
}

void LoadTexture(void* image, int width, int height, int layer, GLenum format)
{
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, width, height, 1, GL_RGBA, format, image);

	if (layer <= 2)
		return;

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

void LoadMipMaps()
{
}

Texture::Texture(const char* file)
{
	int width, height;
	unsigned char* image = SOIL_load_image(file, &width, &height, 0, SOIL_LOAD_RGBA);

	if (!m_TextureArray)
		GenerateTextureArray();

	glBindTexture(GL_TEXTURE_2D_ARRAY, m_TextureArray);
	LoadTexture(image, width, height, m_layers, GL_UNSIGNED_BYTE);
	SOIL_free_image_data(image); // Data given to opengl, dont need it here anymore

	m_ID = m_layers;
	m_layers++;

	m_width = width / (float)MAX_WIDTH;
	m_height = height / (float)MAX_HEIGHT;
}

Texture::Texture(float *pixels, int width, int height)
	: m_width { width / (float)MAX_WIDTH }, m_height { height / (float)MAX_HEIGHT }
{
	if (!m_TextureArray)
		GenerateTextureArray();

	LoadTexture(pixels, width, height, m_layers, GL_FLOAT);
	m_ID = m_layers;
	m_layers++;
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_ID);
}

void Texture::BindArray() 
{ 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_TextureArray); 
}

glm::vec2 Texture::GetBounds()
{
	return glm::vec2(m_width, m_height);
}

///
// Animated Textures
///

AnimatedTexture::AnimatedTexture(const char* file, int spriteWidth, int spriteHeight, int numSpritesX, int numSpritesY)
	: m_spritesX { numSpritesX }, m_spritesY { numSpritesY }
{
	int width, height;
	unsigned char* image = SOIL_load_image(file, &width, &height, 0, SOIL_LOAD_RGBA);

	if (!Texture::m_TextureArray)
		Texture::GenerateTextureArray();

	glBindTexture(GL_TEXTURE_2D_ARRAY, Texture::m_TextureArray);
	LoadTexture(image, width, height, Texture::m_layers, GL_UNSIGNED_BYTE);
	SOIL_free_image_data(image); // Data given to opengl, dont need it here anymore

	m_ID = Texture::m_layers;
	Texture::m_layers++;

	m_width = width / (float)Texture::MAX_WIDTH;
	m_height = height / (float)Texture::MAX_HEIGHT;

	m_spriteWidth = spriteWidth / (float)width * m_width;
	m_spriteHeight = spriteHeight / (float)height * m_height;
}
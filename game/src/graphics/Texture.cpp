/*
FILE: Texture.cpp
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "Universal.h"
#include "Engine\Engine.h"
#include "Mesh.h"
#include "Engine\ResourceManager.h"
#include "Texture.h"
#include "SOIL\SOIL.h"

///
// Texture
///

GLuint Texture::m_TextureArray = 0;
GLuint Texture::m_layers = 0;

void Texture::GenerateTextureArray()
{
	glGenTextures(1, &m_TextureArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_TextureArray);

	// Warning: If larger or more textures are needed, the size cannot exceed GL_MAX_3D_TEXTURE_SIZE 
	// 3 mipmap levels, max layers is 32
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 3, GL_RGBA8, Texture::MAX_WIDTH, Texture::MAX_HEIGHT, 100);
}

void LoadTexture(void* image, int width, int height, int layer, GLenum format)
{
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, width, height, 1, GL_RGBA, format, image);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

Texture::Texture(const char* file)
  : m_ID { m_layers }
{
	int width, height;
	unsigned char* image = SOIL_load_image(file, &width, &height, 0, SOIL_LOAD_RGBA);

	if (!m_TextureArray)
		GenerateTextureArray();

	glBindTexture(GL_TEXTURE_2D_ARRAY, m_TextureArray);
	LoadTexture(image, width, height, m_layers, GL_UNSIGNED_BYTE);
	SOIL_free_image_data(image); // Data given to opengl, dont need it here anymore

	m_layers++;

	m_width = width / (float)MAX_WIDTH;
	m_height = height / (float)MAX_HEIGHT;
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

glm::vec4 Texture::GetBounds()
{
	return glm::vec4(0,0,m_width, m_height);
}

///
// Animated Textures
///

AnimatedTexture::AnimatedTexture(const char* file)
	: Texture { file }, m_spritesX{1}, m_spritesY{1}, m_numSprites{1}, m_spriteWidth{0}, m_spriteHeight{0}
{
}

AnimatedTexture::AnimatedTexture(const char* file, int spriteWidth, int spriteHeight, int spritesX, int spritesY, int numSprites)
	: Texture { file }, m_spritesX { spritesY }, m_spritesY { spritesX }, m_numSprites { numSprites }
{
	m_spriteWidth = spriteWidth / (float)MAX_WIDTH;
	m_spriteHeight = spriteHeight / (float)MAX_HEIGHT;
}

glm::vec4 AnimatedTexture::GetBounds(int frame)
{
	glm::vec2 lower = glm::vec2(m_spriteWidth * (frame % m_spritesX), m_spriteHeight * (frame / m_spritesX));
	return glm::vec4(lower, lower + glm::vec2(m_spriteWidth, m_spriteHeight));
}

void AnimatedTexture::SetSpritesXY(int x, int y, int numSprites)
{
	m_spritesX = x;
	m_spritesY = y;
	if (numSprites == 0)
		numSprites = x * y;
	else
		m_numSprites = numSprites;
}

///
// TextureHandler
///

TextureHandler::TextureHandler(Resource* texture)
{
	SetResource(texture);
}

void TextureHandler::SetResource(Resource* texture)
{
	if (!texture)
		texture = engine->GetResourceManager().Get("default.png");

	m_Texture = texture;

	m_IsAnimated = texture->GetResourceType() == ResourceType::TEXTURE;
	if (m_IsAnimated)
	{
		AnimatedTexture* anim = reinterpret_cast<AnimatedTexture*>(m_Texture->Data());
		m_FPS = anim->GetDefaultFPS();
	}
}

void TextureHandler::Update(float dt)
{
	if (m_IsAnimated)
	{
		m_Timer += dt;
		if (m_Timer > 1.0f / m_FPS)
		{
			// Not adding by 1 prevents skipped frames in low framerates
			m_CurrentFrame += (int)(m_Timer / (1.0f / m_FPS));
			m_Timer = 0;

			if (m_CurrentFrame >= reinterpret_cast<AnimatedTexture*>(m_Texture->Data())->GetMaxFrame())
				m_CurrentFrame = 0;
		}
	}
}

void TextureHandler::SetAnimatedTextureFPS(float fps)
{
	m_FPS = fps;
	m_Timer = 0;
}

void TextureHandler::SetAnimatedTextureFrame(float frame)
{
	m_CurrentFrame = frame;
	m_Timer = 0;
}

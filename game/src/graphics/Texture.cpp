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

#define MAX_LAYERS 32

GLuint Texture::m_TextureArray = 0;
GLuint Texture::m_layers = 0;

std::vector<Texture*> textures;

void Texture::GenerateTextureArray()
{
	glGenTextures(1, &m_TextureArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_TextureArray);

	// Warning: If larger or more textures are needed, the size cannot exceed GL_MAX_3D_TEXTURE_SIZE 
	// 3 mipmap levels, max layers is 32
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 3, GL_RGBA8, Texture::MAX_WIDTH, Texture::MAX_HEIGHT, MAX_LAYERS);
}

void LoadTexture(void* image, int x, int y, int width, int height, int layer, GLenum format)
{
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, x, y, layer, width, height, 1, GL_RGBA, format, image);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
}

std::pair<bool, glm::ivec2> TestPosition(int x, int y, int z, int width, int height)
{
	int rightestX = 0;
	int nextY = Texture::MAX_HEIGHT;
	bool canFit = true;
	for (Texture* tex : textures)
	{
		if (tex->GetLayer() != z)
			continue; // Not on same layer, skip this one

		glm::vec4 tb = tex->GetBounds();
		tb.x *= Texture::MAX_WIDTH;
		tb.y *= Texture::MAX_HEIGHT;
		tb.z *= Texture::MAX_WIDTH;
		tb.w *= Texture::MAX_HEIGHT;

		if (tb.w < nextY && tb.w > y) // Find the lowest value still greater than  y
			nextY = static_cast<int>(tb.w);

		if (y < tb.w && y + height > tb.y) // top or bottom is inside Y bounds, it is overlapping.
		{
			if (tb.z > rightestX) // Get rightmost in these y bounds
				rightestX = static_cast<int>(tb.z);
			if (x < tb.z && x + width > tb.x) // start and/or end is inside X bounds
			{
				canFit = false; // It wont fit, but keep going to find the rightest
			}
		}

	}
	if (x + width > Texture::MAX_WIDTH || y + height > Texture::MAX_HEIGHT)
		canFit = false;

	return std::pair<bool, glm::ivec2>(canFit, glm::ivec2(rightestX,nextY));
}

glm::ivec3 FindLocationForSprite(int width, int height)
{
	// Go through every layer
	for (int z = 0; z < MAX_LAYERS; z++)
	{
		for (int y = 0; y <= Texture::MAX_HEIGHT;)
		{
			std::pair<bool, glm::ivec2> result;
			for (int x = 0; x + width <= Texture::MAX_WIDTH;)
			{
				result = TestPosition(x, y, z, width, height);
				if (result.first)
				{
					// Will fit here
					return glm::vec3(x, y, z);
				}
				if (x == result.second.x + 2) // If x isnt making any progress in x direction
					break;
				x = result.second.x + 2; // + 1 padding pixel
			}
			y = result.second.y + 2;
		}
	}
	//Assert(!"I cant load this image!");
	return glm::ivec3(-1,-1,-1);
}

Texture::Texture(const char* file, bool isTiled)
{
	unsigned char* image = SOIL_load_image(file, &m_pixelWidth, &m_pixelHeight, 0, SOIL_LOAD_RGBA);

	if (!m_TextureArray)
		GenerateTextureArray();

	glm::ivec3 offset = FindLocationForSprite(
		m_pixelWidth == Texture::MAX_WIDTH ? m_pixelWidth : m_pixelWidth + 1,      // Add padding if padding can fit on the page
		m_pixelHeight == Texture::MAX_HEIGHT ? m_pixelHeight : m_pixelHeight + 1);

	m_layer = offset.z;

	glBindTexture(GL_TEXTURE_2D_ARRAY, m_TextureArray);
	LoadTexture(image, offset.x, offset.y, m_pixelWidth, m_pixelHeight, m_layer, GL_UNSIGNED_BYTE);
	SOIL_free_image_data(image); // Data given to opengl, dont need it here anymore

	m_bounds.x = offset.x / (float)MAX_WIDTH;
	m_bounds.y = offset.y / (float)MAX_HEIGHT;
	m_bounds.z = m_bounds.x + m_pixelWidth / (float)MAX_WIDTH;
	m_bounds.w = m_bounds.y + m_pixelHeight / (float)MAX_HEIGHT;
	
	textures.push_back(this);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_layer);
}

void Texture::BindArray() 
{ 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_TextureArray); 
}

glm::vec4 Texture::GetBounds()
{
	return m_bounds;
}

int Texture::PixelWidth()
{
	return m_pixelWidth;
}

int Texture::PixelHeight()
{
	return m_pixelHeight;
}
///
// Animated Textures
///

AnimatedTexture::AnimatedTexture(const char* file)
	: Texture { file }, m_spritesX{1}, m_spritesY{1}, m_numSprites{1}, m_spriteWidth{0}, m_spriteHeight{0}
{
}

AnimatedTexture::AnimatedTexture(const char* file, int spriteWidth, int spriteHeight, int spritesX, int spritesY, int numSprites, float fps)
	: Texture { file }, m_spritesX { spritesX }, m_spritesY { spritesY }, m_numSprites { numSprites }, m_DefaultFPS { fps }
{
	m_spriteWidth = spriteWidth / (float)MAX_WIDTH;
	m_spriteHeight = spriteHeight / (float)MAX_HEIGHT;

	m_pixelWidth /= m_spritesX;
	m_pixelHeight /= m_spritesY;
}

glm::vec4 AnimatedTexture::GetBounds(int frame)
{
	glm::vec2 lower = glm::vec2(m_bounds.x + m_spriteWidth * (frame % m_spritesX), m_bounds.y + m_spriteHeight * (frame / m_spritesX));
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

	m_IsAnimated = texture->GetResourceType() == ResourceType::ANIMATION;
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

glm::vec4 TextureHandler::GetBounds() const
{
	if (m_IsAnimated)
	{
		return reinterpret_cast<AnimatedTexture*>(m_Texture->Data())->GetBounds(m_CurrentFrame);
	}
	return reinterpret_cast<Texture*>(m_Texture->Data())->GetBounds();
}

float TextureHandler::GetTextureAspectRatio()
{
	glm::vec4 bounds = GetBounds();
	return (bounds.z - bounds.x) / (bounds.w - bounds.y); // Width / Height
}
	
void TextureHandler::SetResourceID(ResourceID id)
{
	Resource *resource = engine->GetResourceManager().Get(id);
	SetResource(resource);
}

void TextureHandler::SetAnimatedTextureFPS(float fps)
{
	m_FPS = fps;
	m_Timer = 0;
}

void TextureHandler::SetAnimatedTextureFrame(int frame)
{
	m_CurrentFrame = frame;
	m_Timer = 0;
}

#pragma once
#include <vector>
#include "Texture.h"
#include "GameObjectSystem\TransformComponent.h"
#include "glm\glm.hpp"
#include "Screen.h"

typedef unsigned int GLuint;

enum BACKGROUND_TYPE
{
	BACKGROUND, BACKGROUND_PARALLAX, FOREGROUND, FOREGROUND_PARALLAX
};

class BackgroundComponent
{
public:
	BackgroundComponent(Texture* texture = nullptr, BACKGROUND_TYPE type = BACKGROUND);
	
	// Set speed in x and y directions, and starting sub-texture that will be displayed
	void SetParallax(glm::vec2 minimumPosition, glm::vec2 maximumPosition, glm::vec2 subTextureSize, glm::vec2 subTexturePos);
	void SetTexture(Texture* texture) { m_Texture = texture; }

	bool IsBackground() { return m_Type == BACKGROUND || m_Type == BACKGROUND_PARALLAX; }

	void Render(glm::vec2 pos);

private:
	Texture* m_Texture;
	BACKGROUND_TYPE m_Type;
	glm::vec4 m_ParallaxBounds;
	glm::vec2 m_SubTextureSize = glm::vec2(0.1f,1);
	glm::vec2 m_SubTexturePosition = glm::vec2(0, 0);
	glm::vec2 m_TextureXRange = glm::vec2(0,1);
	glm::vec2 m_TextureYRange = glm::vec2(1,0);

	// Uniform Locations
	static GLuint m_UniTexBox;
	static GLuint m_UniTexLayer;
	static Screen::Mesh* m_Mesh;

	META_REGISTER(BackgroundComponent)
	{
		META_DefineType(BackgroundComponent);
	}
};

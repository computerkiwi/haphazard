#pragma once
#include <vector>
#include "Texture.h"
#include "GameObjectSystem\TransformComponent.h"
#include "glm\glm.hpp"
#include "Screen.h"

typedef unsigned int GLuint;

enum BACKGROUND_TYPE
{
	BACKGROUND, PARALLAX,	FOREGROUND
};

class BackgroundComponent
{
public:
	BackgroundComponent(Texture* texture = nullptr, BACKGROUND_TYPE type = BACKGROUND, int layer = 0);
	
	// Set speed in x and y directions, and starting sub-texture that will be displayed
	void SetParallax(glm::vec2 minimumPosition, glm::vec2 maximumPosition, glm::vec2 subTextureSize);
	void SetTexture(Texture* texture);

	void Render(glm::vec2 pos);

private:
	Texture* m_Texture;
	BACKGROUND_TYPE m_Type;
	int m_Layer;
	glm::vec4 m_ParallaxBounds;
	glm::vec2 m_SubTextureSize = glm::vec2(0.1f,0.1f);
	glm::vec2 m_TextureXRange = glm::vec2(0,1);
	glm::vec2 m_TextureYRange = glm::vec2(0,1);

	// Uniform Locations
	static GLuint m_UniTexBox;
	static GLuint m_UniTexLayer;
	static Screen::Mesh* m_Mesh;
};

#pragma once
#include "glm\glm.hpp"
#include "Texture.h"
#include <vector>
#include "GameObjectSystem\TransformComponent.h"

enum BACKGROUND_TYPE
{
	BACKGROUND, PARALLAX,	FOREGROUND
};

class BackgroundComponent
{
public:
	BackgroundComponent(Texture* texture = nullptr, BACKGROUND_TYPE type = BACKGROUND, int layer = 0);
	
	// Set speed in x and y directions, and starting sub-texture that will be displayed
	void SetParallax(glm::vec2 minimumPosition, glm::vec2 maximumPosition, glm::vec4 subTextureBox);

	void Render(glm::vec2 pos);

private:
	Texture* m_Texture;
	BACKGROUND_TYPE m_Type;
	int m_Layer;
	glm::vec4 m_ParallaxBounds;
	glm::vec4 m_SubTextureBounds;
};

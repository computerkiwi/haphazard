#pragma once
#include <vector>
#include "Texture.h"
#include "GameObjectSystem\TransformComponent.h"
#include "glm\glm.hpp"
#include "Screen.h"

typedef unsigned int GLuint;

enum BACKGROUND_TYPE
{
	// Keep the parallax options after the non parallax
	// Keep them in order
	BACKGROUND = 0, 
	FOREGROUND = 1, 

	BACKGROUND_PARALLAX = 2, 
	FOREGROUND_PARALLAX = 3
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
	// Editor friend
	friend void ImGui_Background(BackgroundComponent *background, GameObject object, Editor *editor);

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
		META_DefineType(BACKGROUND_TYPE);

		META_DefineType(glm::vec4); // HACK
		META_DefineType(glm::vec2); // HACK
		//META_DefineType(Texture *);   // HACK?

		META_DefineType(BackgroundComponent);


		//META_DefineMember(BackgroundComponent, m_Texture, "texture");
		
		META_DefineMember(BackgroundComponent, m_Type, "type");
		
		META_DefineMember(BackgroundComponent, m_ParallaxBounds, "parallaxBounds");
		
		META_DefineMember(BackgroundComponent, m_SubTextureSize, "subTextureSize");
		META_DefineMember(BackgroundComponent, m_SubTexturePosition, "subTexturePosition");
		
		META_DefineMember(BackgroundComponent, m_TextureXRange, "textureXRange");
		META_DefineMember(BackgroundComponent, m_TextureYRange, "textureYRange");
	}
};

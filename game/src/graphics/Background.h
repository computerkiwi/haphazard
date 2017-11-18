#pragma once
#include <vector>
#include "Texture.h"
#include "GameObjectSystem\TransformComponent.h"
#include "glm\glm.hpp"
#include "Screen.h"
#include "Engine/ResourceManager.h"

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
	BackgroundComponent(Resource *res = nullptr, BACKGROUND_TYPE type = BACKGROUND);
	
	// Set speed in x and y directions, and starting sub-texture that will be displayed
	void SetParallax(glm::vec2 minimumPosition, glm::vec2 maximumPosition, glm::vec2 subTextureSize, glm::vec2 subTexturePos);
	void SetTexture(Texture* texture) { m_Texture = texture; }

	bool IsBackground() { return m_Type == BACKGROUND || m_Type == BACKGROUND_PARALLAX; }

	void Render(glm::vec2 pos);

	ResourceID GetResourceID() const;
	void SetResourceID(ResourceID id);

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

	ResourceID m_resID;

	void SetResource(Resource *res);

	// Because meta system can't do enums.
	int GetType() const { return m_Type; }
	void SetType(int type) { m_Type = static_cast<BACKGROUND_TYPE>(type); }

	META_REGISTER(BackgroundComponent)
	{
		// HACK: Doing this because I really need to set up a way to order these.
		META_DefineType(glm::vec4);
		META_DefineType(glm::vec2);

		META_DefineType(BackgroundComponent);

		META_DefineGetterSetter(BackgroundComponent, ResourceID, GetResourceID, SetResourceID, "resourceID");

		META_DefineGetterSetter(BackgroundComponent, int, GetType, SetType, "backgroundType");

		META_DefineMember(BackgroundComponent, m_ParallaxBounds, "parallaxBounds");
		META_DefineMember(BackgroundComponent, m_SubTextureSize, "subTextureSize");
		META_DefineMember(BackgroundComponent, m_SubTexturePosition, "subTexturePosition");
		META_DefineMember(BackgroundComponent, m_TextureXRange, "textureXRange");
		META_DefineMember(BackgroundComponent, m_TextureYRange, "textureYRange");
	}
};

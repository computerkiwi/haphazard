/*
FILE: SpriteComponent.h
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Mesh.h"
#include "meta/meta.h"
#include "Engine/ResourceManager.h"
#include "Engine/Engine.h"

class GameObject;
class SpriteComponent
{
	friend void ImGui_Sprite(SpriteComponent *sprite, GameObject object, Editor * editor);
public:
	// Sprite Constructor for static sprite
	SpriteComponent(Resource *res = NULL);

	void SetTextureResource(Resource *resource);
	void SetTextureID(ResourceID res);

	ResourceID GetResourceID() const { return m_TextureHandler.GetResourceID(); }
	GLuint GetTextureRenderID() const { return m_TextureHandler.GetTexture()->GetLayer(); }

	void SetRenderData(glm::mat4 matrix, std::vector<float>* data);
	void UpdateTextureHandler(float dt);

	void SetColor(glm::vec4 col) { m_Color = col; }

	static Mesh* SpriteMesh() { return m_Mesh; }

private:
	static void SpriteComponent::ConstructUnitMesh();
	
private: // Variables
	static Mesh* m_Mesh;
	TextureHandler m_TextureHandler;
	glm::vec4 m_Color = glm::vec4(1,1,1,1);

	META_REGISTER(SpriteComponent)
	{
		META_DefineMember(SpriteComponent, m_Color, "color");
		META_DefineMember(SpriteComponent, m_TextureHandler, "textureHandler");
	}

};

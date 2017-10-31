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
class SpriteComponent : public Mesh
{
	// Editor access
	friend void ImGui_Sprite(SpriteComponent *sprite, GameObject object);

public:
	// Sprite Constructor for static sprite
	SpriteComponent(Resource *res = NULL);
	// Sprite Contructor for animated sprite
	SpriteComponent(AnimatedTexture* t, float fps);
	
	// Resource functions

	void SetResourceID(ResourceID resID) 
	{
		m_resID = resID;
		SetTextureResource(engine->GetResourceManager().Get(resID));
	}

	ResourceID GetResourceID() const
	{
		return m_resID;
	}

private:
	ResourceID m_resID;

	void SetTextureResource(Resource *resource);

	Texture *GetDefaultTexture()
	{
		static Texture *tex = new Texture("default.png");

		return tex;
	}

	META_REGISTER(SpriteComponent)
	{
		META_DefineType(SpriteComponent);
		META_DefineGetterSetter(SpriteComponent, ResourceID, GetResourceID, SetResourceID, "resourceID");
	}

};

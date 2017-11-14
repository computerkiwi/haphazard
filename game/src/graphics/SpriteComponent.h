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
	friend void ImGui_Sprite(SpriteComponent *sprite, GameObject object, Editor * editor);
public:
	// Sprite Constructor for static sprite
	SpriteComponent(Resource *res = NULL);
	// Sprite Contructor for animated sprite
	SpriteComponent(AnimatedTexture* t, float fps);
	
	// Sets render layer. Higher layers are displayed on top. Default Layer = 0
	void SetLayer(int layer) { m_Layer = layer; }
	int GetLayer() { return m_Layer; }

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
	int m_Layer = 0;

	void SetTextureResource(Resource *resource);
	void SetTextureID(ResourceID res);

	Texture *GetDefaultTexture()
	{
		Resource *res = engine->GetResourceManager().Get("default.png");

		return reinterpret_cast<Texture *>(res->Data());
	}

	META_REGISTER(SpriteComponent)
	{
		META_DefineType(SpriteComponent);
		META_DefineGetterSetter(SpriteComponent, ResourceID, GetResourceID, SetResourceID, "resourceID");
	}

};

#pragma once

#include "Mesh.h"
#include "meta/meta.h"
#include "Engine/ResourceManager.h"
#include "Engine/Engine.h"

class GameObject;
class SpriteComponent : public Mesh
{
	friend void ImGui_Sprite(SpriteComponent *sprite, GameObject object);
public:
	SpriteComponent(Resource *res = NULL);
	SpriteComponent::SpriteComponent(AnimatedTexture* t, float fps);
	
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

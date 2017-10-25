#pragma once

#include "Mesh.h"
#include "meta/meta.h"

class GameObject;
class SpriteComponent : public Mesh
{
	friend void ImGui_Sprite(SpriteComponent *sprite, GameObject object, Editor * editor);
public:
	SpriteComponent(Texture* t = NULL);
	SpriteComponent::SpriteComponent(AnimatedTexture* t, float fps);
	
private:
	Texture *GetDefaultTexture()
	{
		

		return new Texture("default.png");
	}

	META_REGISTER(SpriteComponent)
	{
		META_DefineType(SpriteComponent);
	}

};

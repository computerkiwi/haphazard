#pragma once

#include "Mesh.h"
#include "meta/meta.h"

class GameObject;
class SpriteComponent : public Mesh
{
	friend void ImGui_Sprite(SpriteComponent *sprite, GameObject *object);
public:
	SpriteComponent(Texture* t = NULL);
	SpriteComponent::SpriteComponent(AnimatedTexture* t, float fps);
	
	META_REGISTER(SpriteComponent)
	{
		META_DefineType(SpriteComponent);
	}

};

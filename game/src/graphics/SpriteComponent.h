#pragma once

#include "Mesh.h"
#include "meta/meta.h"

class SpriteComponent : public Mesh
{
public:
	SpriteComponent(Texture* t = NULL);
	SpriteComponent::SpriteComponent(AnimatedTexture* t, float fps);
	
	META_REGISTER(SpriteComponent)
	{
		META_DefineType(SpriteComponent);
	}

};

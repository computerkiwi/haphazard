#pragma once

#include "Mesh.h"

class SpriteComponent : public Mesh
{
public:
	SpriteComponent(Texture* t = NULL);
	SpriteComponent::SpriteComponent(AnimatedTexture* t, float fps);
};

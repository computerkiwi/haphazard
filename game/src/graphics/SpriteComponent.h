#pragma once

#include "Mesh.h"

class SpriteComponent : public Graphics::Mesh
{
public:
	SpriteComponent(Graphics::Texture* t = NULL);
};

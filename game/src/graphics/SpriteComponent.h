#pragma once

#include "Mesh.h"

namespace Graphics
{

	class SpriteComponent : public Graphics::Mesh
	{
	public:
		SpriteComponent(Graphics::Texture* t = NULL);
	};

}

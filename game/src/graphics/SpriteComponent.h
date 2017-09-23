#pragma once

#include "Mesh.h"
#include "meta/meta.h"

namespace Graphics
{

	class SpriteComponent : public Graphics::Mesh
	{
	public:
		SpriteComponent(Graphics::Texture* t = NULL);

		META_NAMESPACE(::Graphics)
		META_REGISTER(SpriteComponent)
		{
			META_DefineType(SpriteComponent);
		}

	};


}

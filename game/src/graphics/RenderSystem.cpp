#include "GameObjectSystem\GameSpace.h"
#include "graphics\SpriteComponent.h"
#include "GameObjectSystem\TransformComponent.h"
#include "Transform.h"

class RenderSystem : public SystemBase
{
	virtual void Init()
	{
	}

	// Called each frame.
	virtual void Update(float dt)
	{
		ComponentMap<Graphics::SpriteComponent> *sprites = GetGameSpace()->GetComponentMap<Graphics::SpriteComponent>();

		for (auto spriteHandle : *sprites)
		{
			ComponentHandle<TransformComponent> transform = spriteHandle.GetSiblingComponent<TransformComponent>();
			if (!transform.IsValid())
			{
				continue;
			}

			//Stuff happens here
			spriteHandle->Draw();
		}
	}

	// Simply returns the default priority for this system.
	virtual size_t DefaultPriority()
	{
		return 0;
	}
};
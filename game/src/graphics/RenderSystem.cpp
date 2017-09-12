#include "GameObjectSystem\GameSpace.h"
#include "graphics\SpriteComponent.h"
#include "GameObjectSystem\Transform.h"
#include "Transform.h"

class RenderSystem : public SystemBase
{
	virtual void Init()
	{
	}

	// Called each frame.
	virtual void Update(float dt)
	{
		ComponentMap<SpriteComponent> *sprites = GetGameSpace()->GetComponentMap<SpriteComponent>();

		for (auto spriteHandle : *sprites)
		{
			ComponentHandle<Transform> transform = spriteHandle.GetSiblingComponent<Transform>();
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
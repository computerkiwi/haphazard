#include "graphics\RenderSystem.h"
#include "GameObjectSystem\GameSpace.h"
#include "graphics\SpriteComponent.h"
#include "GameObjectSystem\TransformComponent.h"
#include "Transform.h"

void RenderSystem::Init()
{
}

// Called each frame.
void RenderSystem::Update(float dt)
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
size_t RenderSystem::DefaultPriority()
{
	return 0;
}
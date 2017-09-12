/*
FILE: Engine.cpp
PRIMARY AUTHOR: Sweet

Engine class data, no work really in here yet.

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Engine.h"

// Component types to register.
#include "GameObjectSystem/TransformComponent.h"
#include "GameObjectSystem/TextSprite.h"
#include "graphics\SpriteComponent.h"
#include "graphics\Texture.h"
#include "graphics\RenderSystem.h"

// GLM didnt have these, huh.
std::ostream& operator<<(std::ostream& os, const glm::mat4& matrix)
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			os << '[' << matrix[j][i] << ']';
		}

		os << std::endl;
	}

	return os;
}

// Systems to register.
// TEMPORARY TEST SYSTEM.
#include <iostream>
class TestSystem : public SystemBase
{
	virtual void Init()
	{
		std::cout << "Test system initialized." << std::endl;
	}

	// Called each frame.
	virtual void Update(float dt)
	{
		ComponentMap<TextSprite> *textSprites = GetGameSpace()->GetComponentMap<TextSprite>();

		for (auto tSpriteHandle : *textSprites)
		{
			ComponentHandle<TransformComponent> transform = tSpriteHandle.GetSiblingComponent<TransformComponent>();
			if (!transform.IsValid())
			{
				continue;
			}

			std::cout << tSpriteHandle->GetName() << ':' << std::endl << transform->Matrix4() << std::endl;
		}
	}

	// Simply returns the default priority for this system.
	virtual size_t DefaultPriority()
	{
		return 0;
	}
};

Engine::Engine()
{
	// Register the component types.
	m_space.registerComponentType<TransformComponent>();
	m_space.registerComponentType<TextSprite>();
	m_space.registerComponentType<Graphics::SpriteComponent>();

	// Register the systems.
	m_space.registerSystem(new TestSystem);
	m_space.registerSystem(new RenderSystem);

	// Initialize the system.
	m_space.Init();

	// TEMPORARY - Creating some GameObjects.
	GameObject obj = m_space.NewGameObject();
	obj.addComponent<TransformComponent>(glm::vec3(0,0,0));
	obj.addComponent<TextSprite>("an object");
	obj.addComponent<Graphics::SpriteComponent>(new Graphics::Texture("bird.png"));

	GameObject obj2 = m_space.NewGameObject();
	obj2.addComponent<TransformComponent>(glm::vec3(1, 2, 3));
	obj2.addComponent<TextSprite>("another object");
	obj.addComponent<Graphics::SpriteComponent>(new Graphics::Texture("bird.png"));

	GameObject obj3 = m_space.NewGameObject();
	obj3.addComponent<TextSprite>("an object without a transform");
	obj.addComponent<Graphics::SpriteComponent>(new Graphics::Texture("bird.png"));
}

void Engine::MainLoop()
{
	while (m_running)
	{
		Update();
	}
}



void Engine::Update()
{
	m_dt = CalculateDt();

	m_space.Update(m_dt);
}


float Engine::CalculateDt()
{
	return (1 / 60.0f);
}



float Engine::Dt() const
{
	return m_dt;
}


lua_State * Engine::GetLua()
{
	return L;
}




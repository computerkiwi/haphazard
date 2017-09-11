/*
FILE: Engine.cpp
PRIMARY AUTHOR: Sweet

Engine class data, no work really in here yet.

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Engine.h"

// Component types to register.
#include "GameObjectSystem/Transform.h"
#include "GameObjectSystem/TextSprite.h"

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
			ComponentHandle<Transform> transform = tSpriteHandle.GetSiblingComponent<Transform>();
			if (!transform.IsValid())
			{
				continue;
			}

			std::cout << tSpriteHandle->GetName() << ':' << transform->xPos << ',' << transform->yPos << ',' << transform->zPos << std::endl;
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
	m_space.registerComponentType<Transform>();
	m_space.registerComponentType<TextSprite>();

	// Register the systems.
	m_space.registerSystem(new TestSystem);

	// Initialize the system.
	m_space.Init();

	// TEMPORARY - Creating some GameObjects.
	GameObject obj = m_space.NewGameObject();
	obj.addComponent<Transform>(1.0f, 2.0f, 3.0f, 4.0f);
	obj.addComponent<TextSprite>("an object");

	GameObject obj2 = m_space.NewGameObject();
	obj2.addComponent<Transform>(5.0f, 5.0f, 5.0f, 90.0f);
	obj2.addComponent<TextSprite>("another object");

	GameObject obj3 = m_space.NewGameObject();
	obj3.addComponent<TextSprite>("an object without a transform");
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




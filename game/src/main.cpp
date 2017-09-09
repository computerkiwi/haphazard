/*
FILE: main.cpp
PRIMARY AUTHOR: Kieran

Entry point into the program.
Currently just a place to play around in.

Copyright � 2017 DigiPen (USA) Corporation.
*/

#include <iostream>
#include "meta/tests.h"
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Engine/Quadtree/Quadtree.h"
#include <memory>
#include "Engine/Engine.h"
#include "meta/example.h"
#include "GameObjectSystem/GameSpace.h"
#include "GameObjectSystem/Transform.h"
#include "GameObjectSystem/TextSprite.h"

#include "Util/Logging.h"

// This comment is useless.

// GLM didnt have these, huh.
std::ostream& operator<<(std::ostream& os,const glm::mat4& matrix)
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			os << matrix[i][j];
		}

		os << std::endl;
	}

	return os;
}

std::ostream& operator<<(std::ostream& os, const glm::vec4& vec)
{
	os << "X: " << vec.x << std::endl;
	os << "Y: " << vec.y << std::endl;
	os << "Z: " << vec.z << std::endl;
	os << "W: " << vec.w << std::endl;

	return os;
}

class TestSystem : public SystemBase
{
	virtual void Init()
	{
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


int main()
{
	
	GameSpace gameSpace;
	gameSpace.registerComponentType<Transform>();
	gameSpace.registerComponentType<TextSprite>();

	gameSpace.registerSystem(std::unique_ptr<TestSystem>(new TestSystem()));

	GameObject obj = gameSpace.NewGameObject();
	obj.addComponent<Transform>(1.0f, 2.0f, 3.0f, 4.0f);
	obj.addComponent<TextSprite>("an object");

	GameObject obj2 = gameSpace.NewGameObject();
	obj2.addComponent<Transform>(5.0f, 5.0f, 5.0f, 90.0f);
	obj2.addComponent<TextSprite>("another object");

	GameObject obj3 = gameSpace.NewGameObject();
	obj3.addComponent<TextSprite>("an object without a transform");

	// QuadTree tree(glm::vec2(-10000, -10000), glm::vec2(10000, 10000));
	// tree.AddChildern();

	Logging::Log("This is a log message with default parameters!");

	Logging::Log("This is a log message with low priority.", Logging::DEFAULT, Logging::LOW_PRIORITY);
	Logging::Log("This is a log message with critical priority!!!!!!!!!", Logging::DEFAULT, Logging::CRITICAL_PRIORITY);

	while (true)
	{
		gameSpace.Update(0.016666f);
	}

	// Keep the console from closing.
	std::cin.ignore();

	return 0;
}

/*
FILE: Engine.cpp
PRIMARY AUTHOR: Sweet

Engine class data, no work really in here yet.

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Engine.h"
#include "../Util/Logging.h"

// Graphics libraries
#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include "graphics\Shaders.h" // Shaders need to be initialized
#include "graphics\Settings.h" // Settings needed for window init

// Component types to register.
#include "GameObjectSystem/TransformComponent.h"
#include "GameObjectSystem/TextSprite.h"
#include "graphics\SpriteComponent.h"
#include "graphics\Texture.h"
#include "graphics\RenderSystem.h"

GLFWwindow* WindowInit(); 

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
		static float time = 0;
		time += dt;

		ComponentMap<TextSprite> *textSprites = GetGameSpace()->GetComponentMap<TextSprite>();

		for (auto tSpriteHandle : *textSprites)
		{
			ComponentHandle<TransformComponent> transform = tSpriteHandle.GetSiblingComponent<TransformComponent>();
			if (!transform.IsValid())
			{
				continue;
			}

			std::cout << tSpriteHandle->GetName() << ':' << std::endl << transform->Matrix4() << std::endl;

			// Show a bunch of transform nonsense.
			transform->Position().x +=sin(time + 2) * 0.03f + 3 * sin(time / 60) / 180;
			transform->Position().y = sin(transform->Position().x * 3);
			transform->Scale().y = 1 + 0.5 * cos(time);
			transform->Rotation() += dt * 50;
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
	//Init OpenGL and start window
	GLFWwindow *window = WindowInit();

	// Load Shaders
	Graphics::Shaders::Init();

	Logging::Init();

	// Register the component types.
	m_space.registerComponentType<TransformComponent>();
	m_space.registerComponentType<TextSprite>();
	m_space.registerComponentType<Graphics::SpriteComponent>();

	// Register the systems.
	m_space.registerSystem(new TestSystem);
	m_space.registerSystem(new RenderSystem(window));

	// Initialize the system.
	m_space.Init();

	// TEMPORARY - Creating some GameObjects.
	GameObject obj = m_space.NewGameObject();
	obj.addComponent<TransformComponent>(glm::vec3(0,0,-1));
	obj.addComponent<TextSprite>("an object");
	obj.addComponent<Graphics::SpriteComponent>(new Graphics::Texture("sampleBlend.png"));

	GameObject obj2 = m_space.NewGameObject();
	obj2.addComponent<TransformComponent>(glm::vec3(-1, 0, 0));
	obj2.addComponent<TextSprite>("another object");
	obj2.addComponent<Graphics::SpriteComponent>(new Graphics::Texture("bird.png"));

	GameObject obj3 = m_space.NewGameObject();
	obj3.addComponent<TextSprite>("an object without a transform");
	obj3.addComponent<Graphics::SpriteComponent>(nullptr);
}

void Engine::MainLoop()
{
	while (m_running)
	{
		Update();
	}
	Logging::Exit();
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

GLFWwindow* WindowInit()
{
	std::cout << "    Initializing glfw... ";

	if (glfwInit() == false)
	{
		glfwTerminate();
		fprintf(stderr, "Could not init GLFW");
		exit(1);
	}

	std::cout << "Initialized glfw" << std::endl;

	glfwWindowHint(GLFW_SAMPLES, 4); //4 MSAA
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(Settings::ScreenWidth(), Settings::ScreenHeight(), "<3", NULL, NULL);

	std::cout << "    Window created" << std::endl;

	if (!window)
	{
		glfwTerminate();
		fprintf(stderr, "Could not create window");
		int i;
		std::cin >> i;
		exit(1);
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		glfwTerminate();
		fprintf(stderr, "Could not init GLEW");
		exit(1);
	}

	return window;
}

/*
FILE: Engine.cpp
PRIMARY AUTHOR: Sweet

Engine class data, no work really in here yet.

Copyright � 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Universal.h"
#include "Engine.h"

// Graphics libraries
#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include "graphics\Shaders.h" // Shaders need to be initialized
#include "graphics\Settings.h" // Settings needed for window init
#include "graphics\Texture.h"

// Component types to register.
#include "GameObjectSystem/TransformComponent.h"
#include "GameObjectSystem/TextSprite.h"
#include "graphics\SpriteComponent.h"
#include "graphics\Texture.h"
#include "graphics\RenderSystem.h"
#include "Physics\RigidBody.h"
#include "Physics\Collider2D.h"

GLFWwindow* WindowInit(); 

// Systems to register.
#include "graphics\RenderSystem.h"
#include "Physics\PhysicsSystem.h"


Engine::Engine()
{
	Logging::Log(Logging::CORE, Logging::LOW_PRIORITY, "Engine constructor called. ");

	//Init OpenGL and start window
	GLFWwindow *window = WindowInit();

	// Load Shaders
	Graphics::Shaders::Init();

	Logging::Init();

	// Register the component types.
	m_space.registerComponentType<TransformComponent>();
	m_space.registerComponentType<TextSprite>();
	m_space.registerComponentType<Graphics::SpriteComponent>();
	m_space.registerComponentType<RigidBodyComponent>();
	m_space.registerComponentType<StaticCollider2DComponent>();
	m_space.registerComponentType<DynamicCollider2DComponent>();

	// Register the systems.
	m_space.registerSystem(new PhysicsSystem);
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

	// RigidBody and Collider Testing Objects
	// object with velocity
	GameObject Brett_obj1 = m_space.NewGameObject();
	Brett_obj1.addComponent<TextSprite>("an object with velocity");
	Brett_obj1.addComponent<TransformComponent>(glm::vec3(0,0,1), glm::vec3(.1f,.1f,1));
	Brett_obj1.addComponent<Graphics::SpriteComponent>(new Graphics::Texture("bird.png"));
	Brett_obj1.addComponent<RigidBodyComponent>(glm::vec3(0,0,0), glm::vec3(.003,.003,0));

	// object with acceleration
	GameObject Brett_obj2 = m_space.NewGameObject();
	Brett_obj2.addComponent<TextSprite>("an object with velocity");
	Brett_obj2.addComponent<TransformComponent>(glm::vec3(0, 0, 1), glm::vec3(.1f, .1f, 1));
	Brett_obj2.addComponent<Graphics::SpriteComponent>(new Graphics::Texture("bird.png"));
	Brett_obj2.addComponent<RigidBodyComponent>(glm::vec3(.0001f, 0, 0), glm::vec3(0, 0, 0));
}

void Engine::MainLoop()
{
	Logging::Log(Logging::CORE, Logging::LOW_PRIORITY, "Entering main loop. ");
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
	Logging::Log(Logging::GRAPHICS, Logging::MEDIUM_PRIORITY, "Initializing glfw...");

	if (glfwInit() == false)
	{
		glfwTerminate();
		fprintf(stderr, "Could not init GLFW");
		exit(1);
	}

	Logging::Log(Logging::GRAPHICS, Logging::MEDIUM_PRIORITY, "Initialized glfw");

	glfwWindowHint(GLFW_SAMPLES, 4); //4 MSAA
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(Settings::ScreenWidth(), Settings::ScreenHeight(), "<3", NULL, NULL);

	Logging::Log(Logging::GRAPHICS, Logging::MEDIUM_PRIORITY, "Window created");

	if (!window)
	{
		glfwTerminate();
		Logging::Log(Logging::GRAPHICS, Logging::CRITICAL_PRIORITY, "Could not create window");
		exit(1);
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		glfwTerminate();
		Logging::Log(Logging::GRAPHICS, Logging::CRITICAL_PRIORITY, "Could not init GLEW");
		exit(1);
	}

	return window;
}

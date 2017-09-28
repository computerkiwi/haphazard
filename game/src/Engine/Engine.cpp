/*
FILE: Engine.cpp
PRIMARY AUTHOR: Sweet

Engine class data, no work really in here yet.

Copyright � 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Universal.h"
#include "Engine.h"

#include "Audio/AudioEngine.h"
#include "meta/meta.h"
#include "../Util/FrameCap.h"

// Graphics libraries
#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include "graphics\Shaders.h" // Shaders need to be initialized
#include "graphics\Settings.h" // Settings needed for window init
#include "graphics\Texture.h"

// imgui
#include <imgui.h>

// Component types to register.
#include "GameObjectSystem/TransformComponent.h"
#include "graphics\SpriteComponent.h"
#include "graphics\Texture.h"
#include "graphics\RenderSystem.h"
#include "Physics\RigidBody.h"
#include "Physics\Collider2D.h"

#include "input\Input.h"

GLFWwindow* WindowInit(); 

// Systems to register.
#include "graphics\RenderSystem.h"
#include "Physics\PhysicsSystem.h"

				   // Init OpenGL and start window
Engine::Engine() : m_window(WindowInit()), m_editor(this, m_window)
{
	Logging::Log(Logging::CORE, Logging::LOW_PRIORITY, "Engine constructor called. ");


	// Load Shaders
	Shaders::Init();

	Logging::Init();
	Audio::Init();
	meta::Init();

	Logging::Log(Logging::CORE, Logging::LOW_PRIORITY, "Engine constructor called. ");

	// Register the component types.
	m_space.RegisterComponentType<TransformComponent>();
	m_space.RegisterComponentType<RigidBodyComponent>();
	m_space.RegisterComponentType<StaticCollider2DComponent>();
	m_space.RegisterComponentType<DynamicCollider2DComponent>();
	m_space.RegisterComponentType<SpriteComponent>();

	// Register the systems.
	m_space.RegisterSystem(new PhysicsSystem);
	m_space.RegisterSystem(new RenderSystem());

	// Initialize the system.
	m_space.Init();

  // TEMPORARY IDK where to put this
  Input::Init(m_window);

	// TEMPORARY - Creating some GameObjects.
	GameObject obj = m_space.NewGameObject();
	obj.AddComponent<TransformComponent>(glm::vec3(0,0,-1));
	obj.AddComponent<SpriteComponent>(new AnimatedTexture("flyboy.png", 240, 314, 5, 4), 60);

	GameObject obj2 = m_space.NewGameObject();
	obj2.AddComponent<TransformComponent>(glm::vec3(-1, 0, 0));
	obj2.AddComponent<SpriteComponent>(new Texture("bird.png"));

	GameObject obj3 = m_space.NewGameObject();
	obj3.AddComponent<SpriteComponent>(nullptr);

	// RigidBody and Collider Testing Objects
	// object with velocity
	GameObject Brett_obj1 = m_space.NewGameObject();
	Brett_obj1.AddComponent<TransformComponent>(glm::vec3(-.25f, 0, 1), glm::vec3(.1f, .1f, 1));
	Brett_obj1.AddComponent<SpriteComponent>(new Texture("bird.png"));
	Brett_obj1.AddComponent<RigidBodyComponent>(glm::vec3(0,0,0), glm::vec3(.000301,.000301,0));
	Brett_obj1.AddComponent<DynamicCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(.1, .1, 0));

	// static colliders: box of cats
	GameObject Brett_obj3 = m_space.NewGameObject();
	Brett_obj3.AddComponent<TransformComponent>(glm::vec3(0, -1, -1), glm::vec3(1, 1, 1));
	Brett_obj3.AddComponent<SpriteComponent>(new Texture("sampleBlend.png"));
	Brett_obj3.AddComponent<StaticCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(1, 1, 0));

	GameObject Brett_obj4 = m_space.NewGameObject();
	Brett_obj4.AddComponent<TransformComponent>(glm::vec3(1, 0, -1), glm::vec3(1, 1, 1));
	Brett_obj4.AddComponent<SpriteComponent>(new Texture("sampleBlend.png"));
	Brett_obj4.AddComponent<StaticCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(1, 1, 0));

	GameObject Brett_obj5 = m_space.NewGameObject();
	Brett_obj5.AddComponent<TransformComponent>(glm::vec3(-1, 0, -1), glm::vec3(1, 1, 1));
	Brett_obj5.AddComponent<SpriteComponent>(new Texture("sampleBlend.png"));
	Brett_obj5.AddComponent<StaticCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(1, 1, 0));

	GameObject Brett_obj6 = m_space.NewGameObject();
	Brett_obj6.AddComponent<TransformComponent>(glm::vec3(0, 1, -1), glm::vec3(1, 1, 1));
	Brett_obj6.AddComponent<SpriteComponent>(new Texture("sampleBlend.png"));
	Brett_obj6.AddComponent<StaticCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(1, 1, 0));
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
	Timer frameCap;

	if (glfwWindowShouldClose(m_window))
		m_running = false;

	m_dt = CalculateDt();
	
	m_space.Update(m_dt);

	Input::Update();

	// Checks 'A' key state
	Input::Input_Debug(Key::B);

	Audio::Update();
	
	
	m_editor.Update();
	
	glfwSwapBuffers(m_window);
	glfwPollEvents();

	frameCap.waitUntil(16666);
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
	glfwSwapInterval(1); // Enable vsync

	if (glewInit() != GLEW_OK)
	{
		glfwTerminate();
		Logging::Log(Logging::GRAPHICS, Logging::CRITICAL_PRIORITY, "Could not init GLEW");
		exit(1);
	}

	glfwSetWindowSizeCallback(window, RenderSystem::ResizeWindowEvent);

	return window;
}

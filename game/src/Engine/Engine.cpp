/*
FILE: Engine.cpp
PRIMARY AUTHOR: Sweet

Engine class data, no work really in here yet.

Copyright (c) 2017 DigiPen (USA) Corporation.
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

// GameObjects
#include "GameObjectSystem/GameObject.h"

// Component types to register.
#include "GameObjectSystem/TransformComponent.h"
#include "graphics\SpriteComponent.h"
#include "graphics\Texture.h"
#include "Physics\RigidBody.h"
#include "Physics\Collider2D.h"
#include "Scripting\ScriptComponent.h"

#include "input\Input.h"

GLFWwindow* WindowInit(); 

// Systems to register.
#include "graphics\RenderSystem.h"
#include "Physics\PhysicsSystem.h"
#include "Scripting\ScriptSystem.h"

extern Engine *engine;


Init_EnginePointer::Init_EnginePointer(Engine *e)
{
	engine = e;
}



				   // Init OpenGL and start window
Engine::Engine() : m_init(this), m_window(WindowInit()), m_editor(this, m_window)
{
	// Load Shaders
	Shaders::Init();

	Audio::Init();
	meta::Init();

	Logging::Log(Logging::CORE, Logging::LOW_PRIORITY, "Engine constructor called. ");

	// Register the component types.
	m_spaces.AddSpace();

	m_spaces[0]->RegisterComponentType<TransformComponent>();
	m_spaces[0]->RegisterComponentType<RigidBodyComponent>();
	m_spaces[0]->RegisterComponentType<StaticCollider2DComponent>();
	m_spaces[0]->RegisterComponentType<DynamicCollider2DComponent>();
	m_spaces[0]->RegisterComponentType<SpriteComponent>();
	m_spaces[0]->RegisterComponentType<ScriptComponent>();

	// Register the systems.
	m_spaces[0]->RegisterSystem(new PhysicsSystem());
	m_spaces[0]->RegisterSystem(new RenderSystem());
	m_spaces[0]->RegisterSystem(new ScriptSystem());

	// Initialize the system.
	m_spaces[0]->Init();

	// TEMPORARY IDK where to put this
	Input::Init(m_window);

	// TEMPORARY - Creating some GameObjects.
	GameObject obj = m_spaces[0]->NewGameObject("Bird");
	obj.AddComponent<TransformComponent>(glm::vec3(0,0,-1));
	obj.AddComponent<SpriteComponent>(new AnimatedTexture("flyboy.png", 240, 314, 5, 4), 60.0f);
	obj.AddComponent<RigidBodyComponent>();
	obj.AddComponent<ScriptComponent>(LuaScript("PlayerController.lua"));

	GameObject obj2 = m_spaces[0]->NewGameObject("Gnome-Bird");
	obj2.AddComponent<TransformComponent>(glm::vec3(-1, 0, 0));
	obj2.AddComponent<SpriteComponent>(new Texture("bird.png"));

	GameObject obj3 = GameObject(m_spaces[0]->NewGameObject("Object 3"));
	obj3.AddComponent<SpriteComponent>(nullptr);

	// RigidBody and Collider Testing Objects
	// object with velocity
	GameObject Brett_obj1 = m_spaces[0]->NewGameObject("Brett Test");
	Brett_obj1.AddComponent<TransformComponent>(glm::vec3(1, 1, 1), glm::vec3(.5f, .5f, 1));
	Brett_obj1.AddComponent<SpriteComponent>(new Texture("bird.png"));
	Brett_obj1.AddComponent<RigidBodyComponent>(glm::vec3(0,0,0), glm::vec3(.6f,0,0));
	Brett_obj1.AddComponent<DynamicCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(.3, .5, 0));

	// object with velocity
	GameObject Brett_obj2 = m_spaces[0]->NewGameObject("Brett Test - Velocity");
	Brett_obj2.AddComponent<TransformComponent>(glm::vec3(2, 1, 1), glm::vec3(.5f, .5f, 1));
	Brett_obj2.AddComponent<SpriteComponent>(new Texture("bird.png"));
	Brett_obj2.AddComponent<RigidBodyComponent>(glm::vec3(0, 0, 0), glm::vec3(-1.2f,0,0));
	Brett_obj2.AddComponent<DynamicCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(.3, .5, 0), collisionLayers::allCollision/*, glm::vec3(1, 0, 0)*/);

	// object on a different collisionLayer
	GameObject Brett_obj4 = m_spaces[0]->NewGameObject("Brett Test - Collision Layer");
	Brett_obj4.AddComponent<TransformComponent>(glm::vec3(1.5, 1.2, 1), glm::vec3(.5f, .5f, 1));
	Brett_obj4.AddComponent<SpriteComponent>(new Texture("bird.png"));
	Brett_obj4.AddComponent<RigidBodyComponent>();
	Brett_obj4.AddComponent<DynamicCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(.3, .5, 0), collisionLayers::decor);

	// static colliders: box of cats
	GameObject Brett_obj3 = m_spaces[0]->NewGameObject("Brett Test - Static Cats");
	Brett_obj3.AddComponent<TransformComponent>(glm::vec3(1.25, -1, -1), glm::vec3(2.5, 1, 1));
	Brett_obj3.AddComponent<SpriteComponent>(new Texture("sampleBlend.png"));
	Brett_obj3.AddComponent<StaticCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(2.5, 1, 0));

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
	
	m_spaces[0]->Update(m_dt);

	Input::Update();

	// Checks 'A' key state
	Input::Input_Debug(Key::B);

	Audio::Update();
	
	if (Input::IsPressed(Key::GRAVE_ACCENT))
		m_editor.ToggleEditor();
	m_editor.Update();
	
	glfwSwapBuffers(m_window);
	glfwPollEvents();

	frameCap.waitUntil(16666);
}


float Engine::CalculateDt()
{
	return (1 / 60.0f);
}


void Engine::LoggingInit()
{
	Logging::Init();
}




float Engine::Dt() const
{
	return m_dt;
}


GLFWwindow* WindowInit()
{
	Logging::Log_StartUp("Initializing glfw...", Logging::GRAPHICS, Logging::MEDIUM_PRIORITY);

	if (glfwInit() == false)
	{
		glfwTerminate();
		fprintf(stderr, "Could not init GLFW");
		exit(1);
	}

	Logging::Log_StartUp("Initialized glfw", Logging::GRAPHICS, Logging::MEDIUM_PRIORITY);

	glfwWindowHint(GLFW_SAMPLES, 4); //4 MSAA
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(Settings::ScreenWidth(), Settings::ScreenHeight(), "<3", NULL, NULL);

	Logging::Log_StartUp("Window created", Logging::GRAPHICS, Logging::MEDIUM_PRIORITY);

	if (!window)
	{
		glfwTerminate();
		Logging::Log_StartUp("Could not create window", Logging::GRAPHICS, Logging::CRITICAL_PRIORITY);
		exit(1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	if (glewInit() != GLEW_OK)
	{
		glfwTerminate();
		Logging::Log_StartUp("Could not init GLEW", Logging::GRAPHICS, Logging::CRITICAL_PRIORITY);
		exit(1);
	}

	glfwSetWindowSizeCallback(window, RenderSystem::ResizeWindowEvent);

	return window;
}

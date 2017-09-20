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

// Graphics libraries
#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include "graphics\Shaders.h" // Shaders need to be initialized
#include "graphics\Settings.h" // Settings needed for window init
#include "graphics\Texture.h"

// imgui
#include <imgui.h>
#include "../Imgui/imgui-setup.h"
#include "../Imgui/Editor.h"

// Component types to register.
#include "GameObjectSystem/TransformComponent.h"
#include "graphics\SpriteComponent.h"
#include "graphics\Texture.h"
#include "graphics\RenderSystem.h"
#include "Physics\RigidBody.h"

GLFWwindow* WindowInit(); 

// Systems to register.
#include "graphics\RenderSystem.h"

				   // Init OpenGL and start window
Engine::Engine() : m_window(WindowInit())
{
	Logging::Log(Logging::CORE, Logging::LOW_PRIORITY, "Engine constructor called. ");


	ImGui_ImplGlfwGL3_Init(m_window, true);

	Editor_Init();
	// Load Shaders
	Graphics::Shaders::Init();

	Logging::Init();
	Audio::Init();
	meta::Init();

	Logging::Log(Logging::CORE, Logging::LOW_PRIORITY, "Engine constructor called. ");

	// Register the component types.
	m_space.RegisterComponentType<TransformComponent>();
	m_space.RegisterComponentType<Graphics::SpriteComponent>();
	m_space.RegisterComponentType<RigidBodyComponent>();

	// Register the systems.
	m_space.RegisterSystem(new RigidBodySystem);
	m_space.RegisterSystem(new RenderSystem());

	// Initialize the system.
	m_space.Init();

	// TEMPORARY - Creating some GameObjects.
	GameObject obj = m_space.NewGameObject();
	obj.AddComponent<TransformComponent>(glm::vec3(0,0,-1));
	obj.AddComponent<Graphics::SpriteComponent>(new Graphics::Texture("sampleBlend.png"));

	GameObject obj2 = m_space.NewGameObject();
	obj2.AddComponent<TransformComponent>(glm::vec3(-1, 0, 0));
	obj2.AddComponent<Graphics::SpriteComponent>(new Graphics::Texture("bird.png"));

	GameObject obj3 = m_space.NewGameObject();
	obj3.addComponent<TransformComponent>(glm::vec3(-1, 0, 0));
	obj3.addComponent<TextSprite>("another object");
	obj3.addComponent<Graphics::SpriteComponent>(new Graphics::Texture("bird.png"));
}

void Engine::MainLoop()
{
	Logging::Log(Logging::CORE, Logging::LOW_PRIORITY, "Entering main loop. ");
	while (m_running)
	{
		Update();
	}
	ImGui_ImplGlfwGL3_Shutdown();
	Logging::Exit();
}



void Engine::Update()
{
	m_dt = CalculateDt();
	
	m_space.Update(m_dt);

	Audio::Update();
	
	ImGui_ImplGlfwGL3_NewFrame();
	Editor(this);
	ImGui::Render();
	
	glfwSwapBuffers(m_window);
	glfwPollEvents();
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

	return window;
}

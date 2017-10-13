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

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

// Temporary.
// TODO: delet this
GameSpace *defaultGameSpacePtr = nullptr;
#include <fstream>

void SerializeGameObject(GameObject &obj, rapidjson::Document& doc)
{
	rapidjson::Value object;
	object = meta::Serialize(obj, doc.GetAllocator());
	doc.PushBack(object, doc.GetAllocator());
}

				   // Init OpenGL and start window
Engine::Engine() : m_window(WindowInit()), m_editor(this, m_window)
{
	// This is temporary for current serialization issues.
	// TODO: delet this
	defaultGameSpacePtr = &m_space;

	Logging::Init();

	// Load Shaders
	Shaders::Init();

	Audio::Init();
	meta::Init();

	Logging::Log(Logging::CORE, Logging::LOW_PRIORITY, "Engine constructor called. ");

	// Register the component types.
	m_space.RegisterComponentType<TransformComponent>();
	m_space.RegisterComponentType<RigidBodyComponent>();
	m_space.RegisterComponentType<StaticCollider2DComponent>();
	m_space.RegisterComponentType<DynamicCollider2DComponent>();
	m_space.RegisterComponentType<SpriteComponent>();
	m_space.RegisterComponentType<ScriptComponent>();

	// Register the systems.
	m_space.RegisterSystem(new PhysicsSystem);
	m_space.RegisterSystem(new RenderSystem());
	m_space.RegisterSystem(new ScriptSystem());

	// Initialize the system.
	m_space.Init();

  // TEMPORARY IDK where to put this
  Input::Init(m_window);


	rapidjson::Document doc;
	doc.SetArray();

	// TEMPORARY - Creating some GameObjects.
	GameObject obj = m_space.NewGameObject();
	obj.AddComponent<TransformComponent>(glm::vec3(0,0,-1));
	obj.AddComponent<SpriteComponent>(new AnimatedTexture("flyboy.png", 240, 314, 5, 4), 60);
	obj.AddComponent<RigidBodyComponent>();
	obj.AddComponent<ScriptComponent>(LuaScript("PlayerController.lua"));
	SerializeGameObject(obj, doc);

	GameObject obj2 = m_space.NewGameObject();
	obj2.AddComponent<TransformComponent>(glm::vec3(-1, 0, 0));
	obj2.AddComponent<SpriteComponent>(new Texture("bird.png"));
	SerializeGameObject(obj2, doc);


	GameObject obj3 = m_space.NewGameObject();
	obj3.AddComponent<SpriteComponent>(nullptr);
	SerializeGameObject(obj3, doc);

	// RigidBody and Collider Testing Objects
	// object with velocity
	GameObject Brett_obj1 = m_space.NewGameObject();
	Brett_obj1.AddComponent<TransformComponent>(glm::vec3(1, 1, 1), glm::vec3(.5f, .5f, 1));
	Brett_obj1.AddComponent<SpriteComponent>(new Texture("bird.png"));
	Brett_obj1.AddComponent<RigidBodyComponent>(glm::vec3(0,0,0), glm::vec3(.6f,0,0));
	Brett_obj1.AddComponent<DynamicCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(.3, .5, 0));
	SerializeGameObject(Brett_obj1, doc);

	// object with velocity
	GameObject Brett_obj2 = m_space.NewGameObject();
	Brett_obj2.AddComponent<TransformComponent>(glm::vec3(2, 1, 1), glm::vec3(.5f, .5f, 1));
	Brett_obj2.AddComponent<SpriteComponent>(new Texture("bird.png"));
	Brett_obj2.AddComponent<RigidBodyComponent>(glm::vec3(0, 0, 0), glm::vec3(-1.2f,0,0));
	Brett_obj2.AddComponent<DynamicCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(.3, .5, 0));
	SerializeGameObject(Brett_obj2, doc);

	// object on a different collisionLayer
	GameObject Brett_obj4 = m_space.NewGameObject();
	Brett_obj4.AddComponent<TransformComponent>(glm::vec3(1.5, 1.2, 1), glm::vec3(.5f, .5f, 1));
	Brett_obj4.AddComponent<SpriteComponent>(new Texture("bird.png"));
	Brett_obj4.AddComponent<RigidBodyComponent>();
	Brett_obj4.AddComponent<DynamicCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(.3, .5, 0), collisionLayers::decor);
	SerializeGameObject(Brett_obj4, doc);

	// static colliders: box of cats
	GameObject Brett_obj3 = m_space.NewGameObject();
	Brett_obj3.AddComponent<TransformComponent>(glm::vec3(1.25, -1, -1), glm::vec3(2.5, 1, 1));
	Brett_obj3.AddComponent<SpriteComponent>(new Texture("sampleBlend.png"));
	Brett_obj3.AddComponent<StaticCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(2.5, 1, 0));
	SerializeGameObject(Brett_obj3, doc);


	rapidjson::StringBuffer s;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
	doc.Accept(writer);
	std::ofstream os("output.json");
	os << s.GetString() << std::endl;
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

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
#include "Util/Serialization.h"

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
	m_resManager.Init();

	// For debug purposes.
	// TODO: Come up with a smarter resource loading strategy.
	m_resManager.LoadAll();

	// Load Shaders
	Shaders::Init();

	Audio::Init();
	meta::Init();
	Input::Init(m_window);

	Logging::Log(Logging::CORE, Logging::LOW_PRIORITY, "Engine constructor called. ");

#define GENERATE_SCENE
#ifdef GENERATE_SCENE

	m_spaces.AddSpace();
	m_spaces.AddSpace();

	m_spaces[1]->RegisterComponentType<ObjectInfo>();
	m_spaces[1]->RegisterComponentType<TransformComponent>();
	m_spaces[1]->RegisterComponentType<RigidBodyComponent>();
	m_spaces[1]->RegisterComponentType<StaticCollider2DComponent>();
	m_spaces[1]->RegisterComponentType<DynamicCollider2DComponent>();
	m_spaces[1]->RegisterComponentType<SpriteComponent>();
	m_spaces[1]->RegisterComponentType<ScriptComponent>();

	// Register the systems.
	m_spaces[0]->RegisterSystem(new PhysicsSystem());
	m_spaces[0]->RegisterSystem(new RenderSystem());
	m_spaces[0]->RegisterSystem(new ScriptSystem());

	m_spaces[1]->RegisterSystem(new PhysicsSystem());
	m_spaces[1]->RegisterSystem(new RenderSystem());
	m_spaces[1]->RegisterSystem(new ScriptSystem());

	// Initialize the system.
	m_spaces[0]->Init();
	m_spaces[1]->Init();


	Resource *tex = m_resManager.Get("bird.png");

	// RigidBody and Collider Testing Objects

	// object with velocity
	GameObject asdf3 = m_spaces[0]->NewGameObject("Parent");
	asdf3.AddComponent<TransformComponent>(glm::vec3(1, 1, 1), glm::vec3(.25f, .25f, 1));
	asdf3.AddComponent<SpriteComponent>(tex);
	asdf3.AddComponent<RigidBodyComponent>(glm::vec3(0, 0, 0), glm::vec3(-.1f, 0, 0));
	asdf3.AddComponent<DynamicCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(.25f, .25f, 0), collisionLayers::player);

	GameObject asdf2 = m_spaces[0]->NewGameObject("Parent");
	asdf2.AddComponent<TransformComponent>(glm::vec3(1, 2, 1), glm::vec3(.25f, .25f, 1));
	asdf2.AddComponent<SpriteComponent>(tex);
	asdf2.AddComponent<RigidBodyComponent>(glm::vec3(0, 0, 0), glm::vec3(-.1f, 0, 0));
	asdf2.AddComponent<DynamicCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(.25f, .25f, 0), collisionLayers::player, .5f);

	// object with velocity
	GameObject asdf1 = m_spaces[0]->NewGameObject("Parent");
	asdf1.AddComponent<TransformComponent>(glm::vec3(-1, 1, 1), glm::vec3(.25f, .25f, 1));
	asdf1.AddComponent<SpriteComponent>(m_resManager.Get("sampleBlend.png"));
	asdf1.AddComponent<RigidBodyComponent>(glm::vec3(0, 0, 0), glm::vec3(.2f, 0, 0));
	asdf1.AddComponent<DynamicCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(.25f, .25f, 0), collisionLayers::player);

	// object with velocity
	GameObject asdf4 = m_spaces[0]->NewGameObject("Parent");
	asdf4.AddComponent<TransformComponent>(glm::vec3(2.5, 1, 1), glm::vec3(.25f, .25f, 1));
	asdf4.AddComponent<SpriteComponent>(m_resManager.Get("sampleBlend.png"));
	asdf4.AddComponent<RigidBodyComponent>(glm::vec3(0, 0, 0), glm::vec3(-.3f, 0, 0));
	asdf4.AddComponent<DynamicCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(.25f, .25f, 0), collisionLayers::player, 0);

	// object with velocity
	GameObject asdf5 = m_spaces[0]->NewGameObject("Parent");
	asdf5.AddComponent<TransformComponent>(glm::vec3(-2.5, 1, 1), glm::vec3(.25f, .25f, 1));
	asdf5.AddComponent<SpriteComponent>(m_resManager.Get("sampleBlend.png"));
	asdf5.AddComponent<RigidBodyComponent>(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));
	asdf5.AddComponent<DynamicCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(.25f, .25f, 0), collisionLayers::player);

	// static colliders: box of cats
	GameObject Brett_obj4 = m_spaces[0]->NewGameObject("Ground");
	Brett_obj4.AddComponent<TransformComponent>(glm::vec3(0, -1, -1), glm::vec3(10, .1, 1));
	Brett_obj4.AddComponent<SpriteComponent>(m_resManager.Get("sampleBlend.png"));
	Brett_obj4.AddComponent<StaticCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(10, .1, 0), collisionLayers::ground, 1, 0);


	this->FileSave("test_out.json");
#else
	this->FileLoad("test_out.json");
	m_spaces[0]->Init();
#endif
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

	Audio::Update();
	
	if (Input::IsPressed(Key::GraveAccent))
		m_editor.ToggleEditor();
	m_editor.Update();
	
	glfwSwapBuffers(m_window);
	glfwPollEvents();

	frameCap.waitUntil(16666);
}


std::string Engine::StringSave()
{
	// Make a document for the allocator.
	// TODO: Figure out how to get an allocator without bothering with a whole document.
	rapidjson::Document doc;

	return JsonToPrettyString(meta::Serialize(*this, doc.GetAllocator()));
}

void Engine::FileSave(const char *fileName)
{
	// Make a document for the allocator.
	// TODO: Figure out how to get an allocator without bothering with a whole document.
	rapidjson::Document doc;

	return JsonToPrettyFile(meta::Serialize(*this, doc.GetAllocator()), fileName);
}

void Engine::StringLoad(const char *jsonString)
{
	rapidjson::Document doc;
	doc.Parse(jsonString);
}

void Engine::FileLoad(const char *fileName)
{
	rapidjson::Document doc = LoadJsonFile(fileName);

	meta::DeserializeAssign(*this, doc);
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


GLFWwindow *WindowInit()
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

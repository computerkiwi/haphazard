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

#include "graphics\Text.h"
#include "graphics\Camera.h"
#include "graphics\Particles.h"

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

	m_spaces.AddSpace();
	m_spaces.AddSpace();

	// Initialize the system.
	m_spaces[0]->Init();
	m_spaces[1]->Init();
	
	const float distance = .5f;
	const float top = 1.5f;

	const float farLeft = -2.0f;
	const float horzDistance = .7f;

	const int allLayer = 6;
	const int noLayer = 0;
	const int playerLayer = 4;
	const int groundLayer = 5;
	const int enemyLayer = 3;
	const int allyPLayer = 2;
	const int decorLayer = 1;



	Resource *tex = m_resManager.Get("bird.png");

	// RigidBody and Collider Testing Objects
	// object with velocity
	GameObject asdf1 = m_spaces[0]->NewGameObject("Parent");
	asdf1.AddComponent<TransformComponent>(glm::vec3(farLeft + (allLayer * horzDistance), top + 1.25f, 1), glm::vec3(.25f, .25f, 1));
	asdf1.AddComponent<SpriteComponent>(tex);
	asdf1.AddComponent<RigidBodyComponent>();
	asdf1.AddComponent<DynamicCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(.25f, .25f, 0), collisionLayers::noCollision);
	asdf1.AddComponent<ScriptComponent>("PlayerController.lua");

	// object with velocity
	GameObject asdf2 = m_spaces[0]->NewGameObject("Parent");
	asdf2.AddComponent<TransformComponent>(glm::vec3(farLeft + (noLayer * horzDistance), top + 1.25f, 1), glm::vec3(.25f, .25f, 1));
	asdf2.AddComponent<SpriteComponent>(tex);
	asdf2.AddComponent<RigidBodyComponent>();
	asdf2.AddComponent<DynamicCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(.25f, .25f, 0), collisionLayers::allCollision);

	// object with velocity
	GameObject asdf3 = m_spaces[0]->NewGameObject("Parent");
	asdf3.AddComponent<TransformComponent>(glm::vec3(farLeft + (playerLayer * horzDistance), top + 1.25f, 1), glm::vec3(.25f, .25f, 1));
	asdf3.AddComponent<SpriteComponent>(tex);
	asdf3.AddComponent<RigidBodyComponent>();
	asdf3.AddComponent<DynamicCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(.25f, .25f, 0), collisionLayers::player);

	// object with velocity
	GameObject asdf4 = m_spaces[0]->NewGameObject("Parent");
	asdf4.AddComponent<TransformComponent>(glm::vec3(farLeft + (groundLayer * horzDistance), top + 1.25f, 1), glm::vec3(.25f, .25f, 1));
	asdf4.AddComponent<SpriteComponent>(tex);
	asdf4.AddComponent<RigidBodyComponent>();
	asdf4.AddComponent<DynamicCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(.25f, .25f, 0), collisionLayers::ground);

	// object with velocity
	GameObject asdf5 = m_spaces[0]->NewGameObject("Parent");
	asdf5.AddComponent<TransformComponent>(glm::vec3(farLeft + (enemyLayer * horzDistance), top + 1.25f, 1), glm::vec3(.25f, .25f, 1));
	asdf5.AddComponent<SpriteComponent>(tex);
	asdf5.AddComponent<RigidBodyComponent>();
	asdf5.AddComponent<DynamicCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(.25f, .25f, 0), collisionLayers::enemy);
	
	GameObject Brett_obj2 = m_spaces[0]->NewGameObject("Child");
	Brett_obj2.AddComponent<TransformComponent>(glm::vec3(0.5f, 0.5f, 1), glm::vec3(.5f, .5f, 1));

	// object with velocity
	GameObject asdf6 = m_spaces[0]->NewGameObject("Parent");
	asdf6.AddComponent<TransformComponent>(glm::vec3(farLeft + (allyPLayer * horzDistance), top + 1.25f, 1), glm::vec3(.25f, .25f, 1));
	asdf6.AddComponent<SpriteComponent>(tex);
	asdf6.AddComponent<RigidBodyComponent>();
	asdf6.AddComponent<DynamicCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(.25f, .25f, 0), collisionLayers::allyProjectile);

	// object with velocity
	GameObject asdf7 = m_spaces[0]->NewGameObject("Parent");
	asdf7.AddComponent<TransformComponent>(glm::vec3(farLeft + (decorLayer * horzDistance), top + 1.25f, 1), glm::vec3(.25f, .25f, 1));
	asdf7.AddComponent<SpriteComponent>(tex);
	asdf7.AddComponent<RigidBodyComponent>();
	asdf7.AddComponent<DynamicCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(.25f, .25f, 0), collisionLayers::decor);


	// static colliders: box of cats
	GameObject Brett_obj1 = m_spaces[0]->NewGameObject("Ground-noCol");
	Brett_obj1.AddComponent<TransformComponent>(glm::vec3(0, top - (noLayer * distance), -1), glm::vec3(10, .1, 1));
	Brett_obj1.AddComponent<SpriteComponent>(m_resManager.Get("sampleBlend.png"));
	Brett_obj1.AddComponent<StaticCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(10, .1, 0), collisionLayers::noCollision);

	// static colliders: box of cats
	GameObject Brett_obj2 = m_spaces[0]->NewGameObject("Ground");
	Brett_obj2.AddComponent<TransformComponent>(glm::vec3(0, top - (allLayer * distance), -1), glm::vec3(10, .1, 1));
	Brett_obj2.AddComponent<SpriteComponent>(m_resManager.Get("sampleBlend.png"));
	Brett_obj2.AddComponent<StaticCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(10, .1, 0), collisionLayers::allCollision);

	// static colliders: box of cats
	GameObject Brett_obj3 = m_spaces[0]->NewGameObject("Ground");
	Brett_obj3.AddComponent<TransformComponent>(glm::vec3(0, top - (playerLayer * distance), -1), glm::vec3(10, .1, 1));
	Brett_obj3.AddComponent<SpriteComponent>(m_resManager.Get("sampleBlend.png"));
	Brett_obj3.AddComponent<StaticCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(10, .1, 0), collisionLayers::player);

	// static colliders: box of cats
	GameObject Brett_obj4 = m_spaces[0]->NewGameObject("Ground");
	Brett_obj4.AddComponent<TransformComponent>(glm::vec3(0, top - (groundLayer * distance), -1), glm::vec3(10, .1, 1));
	Brett_obj4.AddComponent<SpriteComponent>(m_resManager.Get("sampleBlend.png"));
	Brett_obj4.AddComponent<StaticCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(10, .1, 0), collisionLayers::ground);

	// static colliders: box of cats
	GameObject Brett_obj5 = m_spaces[0]->NewGameObject("Ground");
	Brett_obj5.AddComponent<TransformComponent>(glm::vec3(0, top - (enemyLayer * distance), -1), glm::vec3(10, .1, 1));
	Brett_obj5.AddComponent<SpriteComponent>(m_resManager.Get("sampleBlend.png"));
	Brett_obj5.AddComponent<StaticCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(10, .1, 0), collisionLayers::enemy);

	// static colliders: box of cats
	GameObject Brett_obj6 = m_spaces[0]->NewGameObject("Ground");
	Brett_obj6.AddComponent<TransformComponent>(glm::vec3(0, top - (allyPLayer * distance), -1), glm::vec3(10, .1, 1));
	Brett_obj6.AddComponent<SpriteComponent>(m_resManager.Get("sampleBlend.png"));
	Brett_obj6.AddComponent<StaticCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(10, .1, 0), collisionLayers::allyProjectile);

	// static colliders: box of cats
	GameObject Brett_obj7 = m_spaces[0]->NewGameObject("Ground");
	Brett_obj7.AddComponent<TransformComponent>(glm::vec3(0, top - (decorLayer * distance), -1), glm::vec3(10, .1, 1));
	Brett_obj7.AddComponent<SpriteComponent>(m_resManager.Get("sampleBlend.png"));
	Brett_obj7.AddComponent<StaticCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(10, .1, 0), collisionLayers::decor);

	GameObject MainCamera = m_spaces[0]->NewGameObject("Main Camera");
	MainCamera.AddComponent<TransformComponent>(glm::vec3(0,0,0));
	MainCamera.AddComponent<Camera>();
	MainCamera.GetComponent<Camera>()->SetView(glm::vec3(0, 0, 2.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	MainCamera.GetComponent<Camera>()->SetProjection(1.0f, ((float)Settings::ScreenWidth()) / Settings::ScreenHeight(), 1, 10);
	MainCamera.GetComponent<Camera>()->SetPosition(glm::vec2(0, 0));
	MainCamera.GetComponent<Camera>()->SetZoom(3);
	//MainCamera.AddComponent<RigidBodyComponent>();
	//MainCamera.AddComponent<ScriptComponent>("PlayerController.lua");

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

	// m_dt = CalculateDt(); -- Removed for now since only doing (1 / 60.0f)

	m_spaces[0]->Update(m_dt);

	Input::Update();

  Input::InputDebug(Key::A, Key::B, Key::A);

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


float& Engine::GetDtObject()
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

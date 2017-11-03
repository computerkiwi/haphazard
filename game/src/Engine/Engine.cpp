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

#define GENERATE_SCENE 
#ifdef GENERATE_SCENE 

	m_spaces.AddSpace();
	m_spaces.AddSpace();

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
	asdf2.AddComponent<DynamicCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(.25f, .25f, 0), collisionLayers::enemy, .5f);

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
	
	GameObject child = m_spaces[0]->NewGameObject("Child");
	child.AddComponent<TransformComponent>(glm::vec3(0.5f, 0.5f, 1), glm::vec3(.5f, .5f, 1));

	// static colliders: box of cats
	GameObject Brett_obj4 = m_spaces[0]->NewGameObject("Ground");
	Brett_obj4.AddComponent<TransformComponent>(glm::vec3(0, -1, -1), glm::vec3(10, .1, 1));
	Brett_obj4.AddComponent<SpriteComponent>(m_resManager.Get("sampleBlend.png"));
	Brett_obj4.AddComponent<StaticCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(10, .1, 0), collisionLayers::ground, 1, 0);

	asdf1.AddComponent<ParticleSystem>();
	asdf1.GetComponent<ParticleSystem>()->SetAcceleration(glm::vec2(0.5f, 1));
	asdf1.GetComponent<ParticleSystem>()->SetVelocity(glm::vec2(0, 1), glm::vec2(0.05f, 0.2f));
	asdf1.GetComponent<ParticleSystem>()->SetScaleOverLife(glm::vec2(0.1f, 0.1f), glm::vec2(0, 0));
	asdf1.GetComponent<ParticleSystem>()->SetColor(glm::vec4(1,0,0,0.5f), glm::vec4(1,1,0,0));
	asdf1.GetComponent<ParticleSystem>()->SetEmissionRate(0.01f);
	asdf1.GetComponent<ParticleSystem>()->SetParticleLifetime(0.75f);
	asdf1.GetComponent<ParticleSystem>()->SetParticlesPerEmission(2);
	asdf1.GetComponent<ParticleSystem>()->SetEmissionShape(EmissionShape::CIRLCE_EDGE, 0.1f, 0.1f);
	asdf1.GetComponent<ParticleSystem>()->SetHasTrail(false);
	asdf1.GetComponent<ParticleSystem>()->SetTrailLifetime(0.3f);
	asdf1.GetComponent<ParticleSystem>()->SetTrailEmissionRate(0.01f);
	asdf1.GetComponent<ParticleSystem>()->SetTrailColor(glm::vec4(1,0,0,0.5f), glm::vec4(1, 1, 0, 0));
	asdf1.GetComponent<ParticleSystem>()->SetStartRotation(0, 360);
	asdf1.GetComponent<ParticleSystem>()->SetRotationRate(1);
	asdf1.GetComponent<ParticleSystem>()->SetTexture(reinterpret_cast<Texture*>(tex->Data()));
	//asdf1.GetComponent<ParticleSystem>()->SetSimulationSpace(SimulationSpace::LOCAL);

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

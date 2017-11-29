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
#include "graphics\RenderLayer.h"

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
#include "graphics\Background.h"

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

	// Load Shaders
	Shaders::Init();

	Audio::Init();
	meta::Init();
	Input::Init(m_window);

	// For debug purposes.
	// TODO: Come up with a smarter resource loading strategy.
	m_resManager.LoadAll();

	Logging::Log(Logging::CORE, Logging::LOW_PRIORITY, "Engine constructor called. ");


	m_spaces.AddSpace();
	// Initialize the system.
	m_spaces[0]->Init();

	GameObject MainCamera = m_spaces[0]->NewGameObject("Main Camera");
	MainCamera.AddComponent<TransformComponent>(glm::vec3(0, 0, 0), glm::vec3(0.15f));
	MainCamera.AddComponent<Camera>();
	MainCamera.GetComponent<Camera>()->Use();
	MainCamera.GetComponent<Camera>()->SetView(glm::vec3(0, 0, 2.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	MainCamera.GetComponent<Camera>()->SetProjection(1.0f, ((float)Settings::ScreenWidth()) / Settings::ScreenHeight(), 1, 10);
	MainCamera.GetComponent<Camera>()->SetPosition(glm::vec2(0, 0));
	MainCamera.GetComponent<Camera>()->SetZoom(3);
	MainCamera.AddComponent<ScriptComponent>(LuaScript(m_resManager.Get("CameraFollow.lua"), MainCamera));


	Resource *tex = m_resManager.Get("bird.png");
	for (int i = 0; i < 3; ++i)
	{
		GameObject asdf3 = m_spaces[0]->NewGameObject("GodBox");
		asdf3.AddComponent<TransformComponent>(glm::vec3(10000, 1, 0));
		asdf3.AddComponent<SpriteComponent>(tex);
	}

	for (int i = 0; i < 3; ++i)
	{
		const int WIDTH = 4;

		GameObject ground = m_spaces[0]->NewGameObject("Ground");
		ground.AddComponent<TransformComponent>(TransformComponent(glm::vec3(i * WIDTH, -2, 0), glm::vec3(WIDTH, 1, 1)));
		ground.AddComponent<StaticCollider2DComponent>(StaticCollider2DComponent(glm::vec3(WIDTH, 1, 1), collisionLayers::ground, Collider2D::colliderBox));
		ground.AddComponent<SpriteComponent>(m_resManager.Get("ground.png"));
	}

	const int WIDTH = 4;

	GameObject ground = m_spaces[0]->NewGameObject("Ground");
	ground.AddComponent<TransformComponent>(TransformComponent(glm::vec3(3 * WIDTH - .1f, -1.66666f, 0), glm::vec3(WIDTH, 1, 1), 10));
	ground.AddComponent<StaticCollider2DComponent>(StaticCollider2DComponent(glm::vec3(WIDTH, 1, 1), collisionLayers::ground, Collider2D::colliderBox));
	ground.AddComponent<SpriteComponent>(m_resManager.Get("ground.png"));

	GameObject player1 = m_spaces[0]->NewGameObject("Player1");
	player1.AddComponent<TransformComponent>(glm::vec3(0, 0, 1), glm::vec3(0.65, 1, 0));
	player1.AddComponent<RigidBodyComponent>();
	player1.AddComponent<DynamicCollider2DComponent>(DynamicCollider2DComponent(glm::vec3(0.45, 0.71, 0), collisionLayers::player, Collider2D::colliderCapsule));
	player1.AddComponent<SpriteComponent>(m_resManager.Get("GnomeRed.png"));
	player1.AddComponent<ScriptComponent>(LuaScript(m_resManager.Get("PlayerController.lua"), player1));


	GameObject player2 = m_spaces[0]->NewGameObject("Player2");
	player2.AddComponent<TransformComponent>(glm::vec3(1, 0, 0), glm::vec3(0.65, 1, 0));
	player2.AddComponent<RigidBodyComponent>();
	player2.AddComponent<DynamicCollider2DComponent>(DynamicCollider2DComponent(glm::vec3(0.45, 0.71, 0), collisionLayers::player, Collider2D::colliderCapsule));
	player2.AddComponent<SpriteComponent>(m_resManager.Get("GnomeBlue.png"));
	player2.AddComponent<ScriptComponent>(LuaScript(m_resManager.Get("PlayerController.lua"), player2));

	GameObject background = m_spaces[0]->NewGameObject("Background");
	background.AddComponent<BackgroundComponent>(m_resManager.Get("sky.png"), BACKGROUND_PARALLAX);
	background.AddComponent<TransformComponent>(glm::vec3(0, 0, -99));
	background.GetComponent<BackgroundComponent>()->SetParallax(glm::vec2(0, -25), glm::vec2(50, 25), glm::vec2(0.5f, 1.0f), glm::vec2(0, 0.5f));
	background.AddComponent<ScriptComponent>(LuaScript(m_resManager.Get("PlayMusicOnCreate.lua"), background));

	GameObject foreground = m_spaces[0]->NewGameObject("Foreground");
	foreground.AddComponent<BackgroundComponent>(m_resManager.Get("treeboy.png"), FOREGROUND_PARALLAX);
	foreground.AddComponent<TransformComponent>(glm::vec3(0, 0, 10));
	foreground.GetComponent<BackgroundComponent>()->SetParallax(glm::vec2(0, -1), glm::vec2(5.0f, 5), glm::vec2(0.4f, 0.8f), glm::vec2(0, 0.5f));

	GameObject fire = m_spaces[0]->NewGameObject("Fire");
	fire.AddComponent<TransformComponent>(glm::vec3(10, -1, 1));
	fire.AddComponent<ParticleSystem>();
	fire.GetComponent<ParticleSystem>()->SetAcceleration(glm::vec2(0.5f, 0.6f));
	fire.GetComponent<ParticleSystem>()->SetVelocity(glm::vec2(0, 1), glm::vec2(0.05f, 0.2f));
	fire.GetComponent<ParticleSystem>()->SetScaleOverLife(glm::vec2(0.1f, 0.1f), glm::vec2(0, 0));
	fire.GetComponent<ParticleSystem>()->SetColor(glm::vec4(1, 0, 0, 0.75f), glm::vec4(1, 1, 0, 0));
	fire.GetComponent<ParticleSystem>()->SetEmissionRate(0.01f);
	fire.GetComponent<ParticleSystem>()->SetParticleLifetime(1.0f);
	fire.GetComponent<ParticleSystem>()->SetParticlesPerEmission(2);
	fire.GetComponent<ParticleSystem>()->SetEmissionShape(EmissionShape::CIRLCE_EDGE, 0.1f, 0.1f);
	fire.GetComponent<ParticleSystem>()->SetHasTrail(false);
	fire.GetComponent<ParticleSystem>()->SetTrailLifetime(0.3f);
	fire.GetComponent<ParticleSystem>()->SetTrailEmissionRate(0.01f);
	fire.GetComponent<ParticleSystem>()->SetTrailColor(glm::vec4(1, 0, 0, 0.5f), glm::vec4(1, 1, 0, 0));
	fire.GetComponent<ParticleSystem>()->SetStartRotation(0, 360);
	fire.GetComponent<ParticleSystem>()->SetRotationRate(1);

	Screen::GetLayerFrameBuffer(1)->AddEffect(FX::BLOOM);
	Screen::GetLayerFrameBuffer(1)->AddEffect(FX::BLUR);
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

// for recording framerate
float timeCounter = 0;
int frameCounter = 0;

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

	++frameCounter;
	timeCounter += frameCap.timePassed();

	if (timeCounter >= 1000000.0f)
	{
		char windowTitle[128];

		sprintf(windowTitle, "<3   |   FrameRate: %d    Dt: %f", frameCounter, Dt());
		
		glfwSetWindowTitle(m_window, windowTitle);

		frameCounter = 0;
		timeCounter -= 1000000;
	}

	Input::RecordMousePos();
}


std::string Engine::StringSave()
{
	logger << "Saving Game -> String";
	// Make a document for the allocator.
	// TODO: Figure out how to get an allocator without bothering with a whole document.
	rapidjson::Document doc;

	return JsonToPrettyString(meta::Serialize(*this, doc.GetAllocator()));
}

void Engine::FileSave(const char *fileName)
{
	logger << "Saving Game -> File: " << fileName;

	// Make a document for the allocator.
	// TODO: Figure out how to get an allocator without bothering with a whole document.
	rapidjson::Document doc;

	return JsonToPrettyFile(meta::Serialize(*this, doc.GetAllocator()), fileName);
}

void Engine::StringLoad(const char *jsonString)
{
	logger << "Loading game -> String";
	rapidjson::Document doc;
	doc.Parse(jsonString);
}

void Engine::FileLoad(const char *fileName)
{
	logger << "Loading Game -> File: " << fileName;
	rapidjson::Document doc = LoadJsonFile(fileName);

	meta::DeserializeAssign(*this, doc);
}

float Engine::CalculateDt()
{

	static float last = (float)glfwGetTime();

	float currentFrame = (float)glfwGetTime();
	float dt = currentFrame - last;
	last = currentFrame;

	if (m_editor.GetEditorState().show && m_editor.GetEditorState().freeze)
	{
		return 0;
	}

	return dt;
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
	glfwWindowHint(GLFW_REFRESH_RATE, 120);

	// Disable resize
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

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

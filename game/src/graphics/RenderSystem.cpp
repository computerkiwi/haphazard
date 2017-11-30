/*
FILE: RenderSystem.cpp
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "graphics\RenderSystem.h"
#include "GameObjectSystem\GameSpace.h"
#include "graphics\SpriteComponent.h"
#include "GameObjectSystem\TransformComponent.h"
#include "GameObjectSystem\GameObject.h"
#include "Screen.h"
#include "GLFW\glfw3.h"
#include "Camera.h"
#include "Settings.h"
#include "DebugGraphic.h"
#include "Text.h"
#include "Particles.h"
#include <set>
#include <vector>

#include <imgui.h>
#include "Imgui\imgui-setup.h"
#include "Background.h"

#include "RenderLayer.h"
#include "Input\Input.h"
#include "EditorGraphic.h"

static bool resizeCameras = false;
static int width;
static int height;
static int currLayer;

void ToggleFullscreen();

RenderSystem::RenderSystem()
{
}

void RenderSystem::Init()
{
	glDisable(GL_MULTISAMPLE);
	glDisable(GL_DEPTH_TEST); // Don't need depth for 2D. Render things in order.
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Screen::InitScreen();
	Font::InitFonts();
}

void RenderSystem::UpdateCameras(float dt)
{
	ComponentMap<Camera> *cameras = GetGameSpace()->GetComponentMap<Camera>();

	int numCameras = 0;
	for (auto& camera : *cameras)
	{
		numCameras++;
		if (!camera->IsActiveCam()) // Only update the active camera
			continue;

		// Check for valid transform
		ComponentHandle<TransformComponent> transform = camera.GetSiblingComponent<TransformComponent>();
		if (!transform.IsValid())
		{
			continue;
		}
		//Update Cameras

		camera->SetAspectRatio(width / (float)height);
		camera->SetPosition(transform->GetPosition());
		camera->SetRotation(transform->GetRotation());
	}
	resizeCameras = false;

	if (numCameras == 0)
	{
		Logging::Log(Logging::GRAPHICS, Logging::MEDIUM_PRIORITY, "There are no cameras in the scene!");
	}
	else if(Camera::GetActiveCamera())
		Camera::GetActiveCamera()->Use();
}

void RenderSystem::RenderSprites(float dt)
{
	ComponentMap<SpriteComponent> *sprites = GetGameSpace()->GetComponentMap<SpriteComponent>();

	if (sprites->begin() == sprites->end())
		return;

	// Instancing variables
	std::vector<float> data;
	std::vector<int> tex;
	int numMeshes = 0;

	// All sprites will be the same mesh, so numVerts only has to be set once
	static const int numVerts = SpriteComponent::SpriteMesh()->NumVerts();

	std::set<int> layers;
	for (auto& spriteHandle : *sprites)
	{
		ComponentHandle<TransformComponent> transform = spriteHandle.GetSiblingComponent<TransformComponent>();
		if (!transform.IsValid())
		{
			continue;
		}

		layers.insert(static_cast<int>(transform->GetZLayer()));
	}

	int last = -1234;
	for (int layer : layers)
	{
		// Use this layer (garunteed to not be a duplicate layer because layers is a set)
		Screen::GetLayerFrameBuffer(layer)->Use();

		data.clear();
		tex.clear();

		for (auto& spriteHandle : *sprites)
		{
			// Check for valid transform
			ComponentHandle<TransformComponent> transform = spriteHandle.GetSiblingComponent<TransformComponent>();
			if (!transform.IsValid())
			{
				continue;
			}

			if (static_cast<int>(transform->GetZLayer()) != layer)
				continue;

			// Update animated sprites
			spriteHandle->UpdateTextureHandler(dt);

			// Places vertex data into data vector to be used in Vertex VBO
			spriteHandle->SetRenderData(transform.Get(), &data);

			// Places texture in tex vector to be used in Texture VBO
			tex.push_back(spriteHandle->GetTextureRenderID());

			// Keep count of all meshes used in instancing call
			numMeshes++;
		}

		if (numMeshes == 0)
			return;

		// Bind sprite shader
		Shaders::spriteShader->Use();

		// Bind buffers and set instance data of all sprites
		Mesh::BindTextureVBO();
		glBufferData(GL_ARRAY_BUFFER, sizeof(int) * tex.size(), tex.data(), GL_STATIC_DRAW);

		Mesh::BindInstanceVBO();
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), data.data(), GL_STATIC_DRAW);

		// Bind first sprite VAO, all sprite have the same vertex attributes
		SpriteComponent::SpriteMesh()->BindVAO();

		// Draw all sprites in this layer
		glDrawArraysInstanced(GL_TRIANGLES, 0, numMeshes * numVerts, numMeshes);
	}
}

void RenderSystem::RenderText(float dt)
{
	ComponentMap<TextComponent> *text = GetGameSpace()->GetComponentMap<TextComponent>();
	int lastLayer = -999;

	for (auto& textHandle : *text)
	{
		// Check for valid transform
		ComponentHandle<TransformComponent> transform = textHandle.GetSiblingComponent<TransformComponent>();
		if (!transform.IsValid())
		{
			continue;
		}

		int layer = static_cast<int>(transform->GetZLayer());
		if (lastLayer != layer)
		{
			Screen::GetLayerFrameBuffer(layer)->Use();
			lastLayer = layer;
		}

		textHandle->Draw(transform->GetMatrix4());
	}
}

void RenderSystem::RenderParticles(float dt)
{
	ComponentMap<ParticleSystem> *particles = GetGameSpace()->GetComponentMap<ParticleSystem>();
	int lastLayer = -999;

	for (auto& particleHandle : *particles)
	{
		ComponentHandle<TransformComponent> transform = particleHandle.GetSiblingComponent<TransformComponent>();
		if (!transform.IsValid())
		{
			continue;
		}

		int layer = static_cast<int>(transform->GetZLayer());
		if (lastLayer != layer)
		{
			Screen::GetLayerFrameBuffer(layer)->Use();
			lastLayer = layer;
		}

		particleHandle->Render(dt, transform->GetPosition());
	}
}

void RenderSystem::RenderBackgrounds(float dt)
{
	glm::vec2 camPos = Camera::GetActiveCamera()->GetPosition();
	int lastLayer = -999;

	ComponentMap<BackgroundComponent> *backgrounds = GetGameSpace()->GetComponentMap<BackgroundComponent>();
	for (auto& bgHandle : *backgrounds)
	{
		ComponentHandle<TransformComponent> transform = bgHandle.GetSiblingComponent<TransformComponent>();
		if (!transform.IsValid())
		{
			continue;
		}

		int layer = static_cast<int>(transform->GetZLayer());
		if (lastLayer != layer)
		{
			Screen::GetLayerFrameBuffer(layer)->Use();
			lastLayer = layer;
		}

		if(bgHandle->IsBackground())
			bgHandle->Render(camPos);
	}
}

void RenderSystem::RenderForegrounds(float dt)
{
	glm::vec2 camPos = Camera::GetActiveCamera()->GetPosition();
	int lastLayer = -999;

	ComponentMap<BackgroundComponent> *backgrounds = GetGameSpace()->GetComponentMap<BackgroundComponent>();
	for (auto& bgHandle : *backgrounds)
	{
		ComponentHandle<TransformComponent> transform = bgHandle.GetSiblingComponent<TransformComponent>();
		if (!transform.IsValid())
		{
			continue;
		}

		int layer = static_cast<int>(transform->GetZLayer());
		if (lastLayer != layer)
		{
			Screen::GetLayerFrameBuffer(layer)->Use();
			lastLayer = layer;
		}

		if (!bgHandle->IsBackground())
			bgHandle->Render(camPos);
	}
}

// Called each frame.
void RenderSystem::Update(float dt)
{
	if (Input::IsPressed(Key::F11))
	{
		ToggleFullscreen();
	}

	// Clear screen and sets correct framebuffer
	Screen::Use();

	//Start Loop
	UpdateCameras(dt);
	RenderBackgrounds(dt);
	RenderSprites(dt);
	RenderText(dt);
	RenderParticles(dt);
	RenderForegrounds(dt);

	//End loop
	Screen::Draw(); // Draw to screen and apply post processing effects
	DebugGraphic::DrawAll();
}

void RenderSystem::ResizeWindowEvent(GLFWwindow* window, int w, int h)
{
	width = w;
	height = h;

	Settings::SetScreenSize(w, h);

	Screen::ResizeScreen(width, height);
	resizeCameras = true;
	engine->GetEditor()->ResizeEvent(w, h);
}

SystemBase *RenderSystem::NewDuplicate()
{
	return new RenderSystem(*this);
}

// Simply returns the default priority for this system.
size_t RenderSystem::DefaultPriority()
{
	return 999;
}

void ToggleFullscreen()
{
	static bool isFullscreen = false;

	GLFWmonitor* primary = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(primary);
	if (!isFullscreen)
	{
		// Resize to fullscreen (store old resolution somewhere)
		glfwSetWindowMonitor(engine->GetWindow(), primary, 0, 0, mode->width, mode->height, mode->refreshRate);
		isFullscreen = true;
	}
	else
	{
		// Resize to window (with w=800,h=600)
		glfwSetWindowMonitor(engine->GetWindow(), NULL, mode->width / 2 - 400, mode->height / 2 - 320, 800, 600, mode->refreshRate);
		isFullscreen = false;
	}
}
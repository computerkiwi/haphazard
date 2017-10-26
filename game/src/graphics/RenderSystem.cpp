#include "graphics\RenderSystem.h"
#include "GameObjectSystem\GameSpace.h"
#include "graphics\SpriteComponent.h"
#include "GameObjectSystem\TransformComponent.h"
#include "GameObjectSystem\GameObject.h"
#include "Transform.h"
#include "Screen.h"
#include "GLFW\glfw3.h"
#include "Camera.h"
#include "Settings.h"
#include "DebugGraphic.h"

#include <imgui.h>
#include "Imgui\imgui-setup.h"

RenderSystem::RenderSystem()
{
}

static Camera* mainCamera;

void RenderSystem::Init()
{
	mainCamera = new Camera();
	mainCamera->SetView(glm::vec3(0, 0, 2.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	mainCamera->SetProjection(1.0f, ((float)Settings::ScreenWidth()) / Settings::ScreenHeight(), 1, 10);
	mainCamera->SetPosition(glm::vec3(0, 0, 2.0f));

	mainCamera->SetZoom(3);
}

// Called each frame.
void RenderSystem::Update(float dt)
{
	Screen::GetView().Use();
	//Screen::UpdateRaindrops(dt);
	////Start Loop

	//Screen::GetView().ResizeScreen(1920, 1080);

	ComponentMap<SpriteComponent> *sprites = GetGameSpace()->GetComponentMap<SpriteComponent>();

	std::vector<float> data;
	std::vector<int> tex;
	int numMeshes = 0;
	int numVerts = 0;

	Shaders::defaultShader->Use();

	for (auto& spriteHandle : *sprites)
	{
		ComponentHandle<TransformComponent> transform = spriteHandle.GetSiblingComponent<TransformComponent>();
		if (!transform.IsValid())
		{
			continue;
		}

		spriteHandle->UpdateAnimatedTexture(dt);

		spriteHandle->SetRenderData(transform->GetMatrix4(), &data);
		tex.push_back(spriteHandle->GetRenderTextureID());

		numMeshes++;

		if (numVerts == 0)
			numVerts = spriteHandle->NumVerts();

		//Stuff happens here
	}

	Mesh::BindTextureVBO();
	glBufferData(GL_ARRAY_BUFFER, sizeof(int) * tex.size(), tex.data(), GL_STATIC_DRAW);

	Mesh::BindInstanceVBO();
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), data.data(), GL_STATIC_DRAW);

	sprites = GetGameSpace()->GetComponentMap<SpriteComponent>();
	// Bind first VAO, all VAOs should be the same until multiple shaders are used for sprites
	sprites->begin()->BindVAO();

	glDrawArraysInstanced(GL_TRIANGLES, 0, numMeshes * numVerts, numMeshes);
	

	//End loop
	glBlendFunc(GL_ONE, GL_ZERO);

	DebugGraphic::DrawAll();
	Screen::GetView().Draw();
}

void RenderSystem::ResizeWindowEvent(GLFWwindow* window, int width, int height)
{
	Screen::GetView().ResizeScreen(width, height);
	mainCamera->SetAspectRatio(width / (float)height);
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
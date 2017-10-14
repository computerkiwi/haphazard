#include "graphics\RenderSystem.h"
#include "GameObjectSystem\GameSpace.h"
#include "graphics\SpriteComponent.h"
#include "GameObjectSystem\TransformComponent.h"
#include "Transform.h"
#include "Screen.h"
#include "GLFW\glfw3.h"
#include "Camera.h"
#include "Settings.h"
#include "DebugGraphic.h"
#include "Text.h"

#include <imgui.h>
#include "Imgui\imgui-setup.h"

static bool resizeCameras = false;
static int width;
static int height;

RenderSystem::RenderSystem()
{
}

void RenderSystem::Init()
{
	Font::InitFonts();

//	Screen::GetView().AddEffect(FX::EDGE_DETECTION);
//	Screen::GetView().AddEffect(FX::BLOOM);
//	Screen::GetView().SetBlurAmount(0.9f);
}

// Called each frame.
void RenderSystem::Update(float dt)
{
	Screen::GetView().Use();
	//Screen::UpdateRaindrops(dt);
	////Start Loop

	if (resizeCameras)
	{
		ComponentMap<Camera> *cameras = GetGameSpace()->GetComponentMap<Camera>();

		for (auto& camera : *cameras)
		{
			camera->SetAspectRatio(width / (float)height);
		}

		resizeCameras = false;
	}

	ComponentMap<SpriteComponent> *sprites = GetGameSpace()->GetComponentMap<SpriteComponent>();

	std::vector<float> data;
	std::vector<int> tex;
	int numMeshes = 0;
	int numVerts = 0;

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
		//t->Draw(transform->GetMatrix4());
	}

	Shaders::defaultShader->Use();
	DebugGraphic::DrawShape(glm::vec2(1, 0), glm::vec2(0.25f, 0.25f), 3.14f / 4, glm::vec4(1, 0, 1, 1));
	
	Mesh::BindTextureVBO();
	glBufferData(GL_ARRAY_BUFFER, sizeof(int) * tex.size(), tex.data(), GL_STATIC_DRAW);

	Mesh::BindInstanceVBO();
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), data.data(), GL_STATIC_DRAW);

	// Bind first VAO, all VAOs should be the same until multiple shaders are used for sprites
	sprites->begin()->BindVAO();

	glDrawArraysInstanced(GL_TRIANGLES, 0, numMeshes * numVerts, numMeshes);
	
	ComponentMap<TextComponent> *text = GetGameSpace()->GetComponentMap<TextComponent>();

	for (auto& textHandle : *text)
	{
		ComponentHandle<TransformComponent> transform = textHandle.GetSiblingComponent<TransformComponent>();
		if (!transform.IsValid())
		{
			continue;
		}
		textHandle->Draw(transform->GetMatrix4());
	}

	//End loop
	glBlendFunc(GL_ONE, GL_ZERO);

	DebugGraphic::DrawAll();
	Screen::GetView().Draw();
}

void RenderSystem::ResizeWindowEvent(GLFWwindow* window, int w, int h)
{
	width = w;
	height = h;

	Screen::GetView().ResizeScreen(width, height);
	resizeCameras = true;
}

// Simply returns the default priority for this system.
size_t RenderSystem::DefaultPriority()
{
	return 999;
}

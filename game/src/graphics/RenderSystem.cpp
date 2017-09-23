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

#include <imgui.h>
#include "Imgui\imgui-setup.h"

RenderSystem::RenderSystem()
{
}

using namespace Graphics;

static Camera* mainCamera;

DebugGraphic* c;

void RenderSystem::Init()
{
	mainCamera = new Camera();
	mainCamera->SetView(glm::vec3(0, 0, 2.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	mainCamera->SetProjection(1.0f, ((float)Settings::ScreenWidth()) / Settings::ScreenHeight(), 1, 10);
	mainCamera->SetPosition(glm::vec3(0, 0, 2.0f));
}

// Called each frame.
void RenderSystem::Update(float dt)
{
	glEnableVertexAttribArray(0);
	Graphics::Screen::GetView().Use();
	Graphics::Screen::UpdateRaindrops(dt);
	////Start Loop

	ComponentMap<Graphics::SpriteComponent> *sprites = GetGameSpace()->GetComponentMap<Graphics::SpriteComponent>();

	for (auto spriteHandle : *sprites)
	{
		ComponentHandle<TransformComponent> transform = spriteHandle.GetSiblingComponent<TransformComponent>();
		if (!transform.IsValid())
		{
			continue;
		}

		//Stuff happens here
		spriteHandle->Draw(transform->GetMatrix4());

		mainCamera->SetZoom(3);
		
		DebugGraphic::DrawShape(glm::vec2(1, 0), glm::vec2(0.25f,0.25f), 3.14/4, glm::vec4(1,0,1,1));

	}


	//End loop
	glBlendFunc(GL_ONE, GL_ZERO);
	Graphics::Screen::GetView().Draw();

	DebugGraphic::DrawAll();

	glDisableVertexAttribArray(0);
}

// Simply returns the default priority for this system.
size_t RenderSystem::DefaultPriority()
{
	return 999;
}
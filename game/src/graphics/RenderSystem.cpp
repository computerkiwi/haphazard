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

RenderSystem::RenderSystem(GLFWwindow* window)
	: m_window(window)
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

	DebugGraphic* d = new DebugGraphic(glm::vec2(1, 0));
	DebugGraphic* a = new DebugGraphic(glm::vec2(2, 1));
	DebugGraphic* b = new DebugGraphic(glm::vec2(-2, -2));
	c = new DebugGraphic(glm::vec2(1, 0), glm::vec2(0.5f,0.5f) );
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
		spriteHandle->Draw(transform->Matrix4());

		mainCamera->SetZoom(3);
		c->SetScale(c->GetScale().x + dt, 0.5f);
		c->SetColor(0, 0, 1, 1);

		//mainCamera->SetPosition(mainCamera->GetPosition() + glm::vec3(dt,0,0));
	}


	//End loop
	glBlendFunc(GL_ONE, GL_ZERO);
	Graphics::Screen::GetView().Draw();

	DebugGraphic::DrawAll();

	glDisableVertexAttribArray(0);
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

// Simply returns the default priority for this system.
size_t RenderSystem::DefaultPriority()
{
	return 999;
}
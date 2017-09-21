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

static Camera* mainCamera;

DebugGraphic* c;

void RenderSystem::Init()
{
	mainCamera = new Camera();
	mainCamera->SetView(glm::vec3(0, 0, 2.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	mainCamera->SetProjection(1.0f, ((float)Settings::ScreenWidth()) / Settings::ScreenHeight(), 1, 10);
	mainCamera->SetPosition(glm::vec3(0, 0, 2.0f));

	//mainCamera->ResizeViewport(1920, 1080);
}

// Called each frame.
void RenderSystem::Update(float dt)
{
	glEnableVertexAttribArray(0);
	Screen::GetView().Use();
	Screen::UpdateRaindrops(dt);
	////Start Loop

	//Screen::GetView().ResizeScreen(1920, 1080);

	ComponentMap<SpriteComponent> *sprites = GetGameSpace()->GetComponentMap<SpriteComponent>();

	std::vector<float> data;
	int numMeshes = 0;

	Mesh::BindVBO();

	Shaders::defaultShader->Use();

	for (auto spriteHandle : *sprites)
	{
		ComponentHandle<TransformComponent> transform = spriteHandle.GetSiblingComponent<TransformComponent>();
		if (!transform.IsValid())
		{
			continue;
		}

		spriteHandle->UpdateAnimatedTexture(dt);
		spriteHandle->Draw(transform->Matrix4(), &data);

		numMeshes++;

		//Stuff happens here
		mainCamera->SetZoom(3);
		DebugGraphic::DrawShape(glm::vec2(1, 0), glm::vec2(0.25f,0.25f), 3.14/4, glm::vec4(1,0,1,1));
	}

	if(data.size() > 0)
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), data.data(), GL_STATIC_DRAW);

	sprites = GetGameSpace()->GetComponentMap<SpriteComponent>();

	int i = 0;
	for (auto spriteHandle : *sprites)
	{
		spriteHandle->BindVAO();
		spriteHandle->BindTexture();
		glDrawArraysInstanced(GL_TRIANGLES, 0, spriteHandle->NumVerts() * sizeof(float), numMeshes);
		i++;
	}

	//End loop
	glBlendFunc(GL_ONE, GL_ZERO);
	Screen::GetView().Draw();

	DebugGraphic::DrawAll();

	glDisableVertexAttribArray(0);
	glfwSwapBuffers(m_window);
	glfwPollEvents();

	//data.clear();
}

// Simply returns the default priority for this system.
size_t RenderSystem::DefaultPriority()
{
	return 999;
}
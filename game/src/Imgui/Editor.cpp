

#include "Editor.h"
#include "imgui-setup.h"

#include <GL/glew.h>
#include <GLFW\glfw3.h>
#include <string>
#include <sstream>


#include "GameObjectSystem\GameSpace.h"
#include "GameObjectSystem\TextSprite.h"
#include "GameObjectSystem\TransformComponent.h"

#include "Util\Logging.h"

void TopBar();
void Console();
void ImGui_GameObject(GameObject *object);
void ImGui_Transform(TransformComponent *transform);
void ImGui_TextSprite(TextSprite *tsprite);

void Editor()
{
	// TopBar();

	// ImGui::ShowTestWindow();

	ImGui_GameObject(nullptr);
}


void TopBar()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save"))
			{
				Logging::Log("Saving");
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
}


void Console()
{
	ImGui::SetNextWindowPosCenter();
	char command_buffer[1024];
	ImGui::Begin("Console");
	ImGui::InputText("Command: ", command_buffer, 300);

	std::stringstream command(command_buffer);
	std::string parse;

	while (command >> parse)
	{
		if (parse == "Log")
		{
			Logging::Log("Hello!");
		}
	}


	ImGui::End();
}


void ImGui_GameObject(GameObject *object)
{
	if (object)
	{
		std::string name("GameObject - ");
		name += std::to_string(object->Getid());

		ImGui::SetNextWindowSize(ImVec2(325, 400));
		ImGui::SetNextWindowPos(ImVec2(15, 25), ImGuiCond_Once);
		ImGui::Begin(name.c_str(), nullptr, ImGuiWindowFlags_NoResize);



		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::ImColor(0.25f, 0.55f, 0.9f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::ImColor(0.0f, 0.45f, 0.9f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::ImColor(0.25f, 0.25f, 0.9f));
		if (ImGui::Button("Duplicate"))
		{
			object->Duplicate<dummy>();
		}
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{

		}

		// if object - > component
		// ImGui_Component(ComponetType *component);
		if (object->getComponent<TransformComponent>().Get())
		{
			ImGui_Transform(object->getComponent<TransformComponent>().Get());
		}

		if (object->getComponent<TextSprite>().Get())
		{
			ImGui_TextSprite(object->getComponent<TextSprite>().Get());
		}

		ImGui::End();
	}
	else
	{
		std::string name("GameObject - ");
		name += "No Object Selected";

		ImGui::SetNextWindowSize(ImVec2(325, 400));
		ImGui::SetNextWindowPos(ImVec2(15, 35), ImGuiCond_Once);
		ImGui::Begin(name.c_str(), nullptr, ImGuiWindowFlags_NoResize);
		ImGui::End();
	}
}


void ImGui_Transform(TransformComponent *transform)
{
	if (ImGui::CollapsingHeader("Transform Component"))
	{
		ImGui::PushItemWidth(120);
		if (ImGui::CollapsingHeader("Position"))
		{
			ImGui::InputFloat(" X", &transform->GetPosition().x);
			ImGui::InputFloat(" Y", &transform->GetPosition().x);
			// ImGui::InputFloat(" Z", &z); - Add for 3d
			ImGui::PopItemWidth();			
		}
		if (ImGui::CollapsingHeader("Scale"))
		{
			ImGui::InputFloat(" X", &transform->GetScale().x);
			ImGui::InputFloat(" Y", &transform->GetScale().y);
			ImGui::Separator();
		}
		ImGui::PopItemWidth();
		
		ImGui::InputFloat("Rotation", &transform->GetRotation());
	}
}


void ImGui_TextSprite(TextSprite *tsprite)
{
	if (ImGui::CollapsingHeader("Text Sprite"))
	{
		ImGui::Separator();
	}
}


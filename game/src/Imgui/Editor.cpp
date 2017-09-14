

#include "Editor.h"
#include "imgui-setup.h"

#include <GL/glew.h>
#include <GLFW\glfw3.h>
#include <string>
#include <sstream>

#include "GameObjectSystem\GameObject.h"
#include "Util\Logging.h"

void Console();
void ImGui_GameObject();
void ImGui_Transform();

void Editor()
{
	bool show_test_window = true;
	bool show_another_window = false;

	glfwPollEvents();
	ImGui_ImplGlfwGL3_NewFrame();

	std::string console;

	ImGui::ShowTestWindow();

	ImGui_GameObject();
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


void ImGui_GameObject()
{
	std::string name("GameObject - ");
	name += std::to_string(0);

	ImGui::SetNextWindowSize(ImVec2(325, 400));
	ImGui::SetNextWindowPos(ImVec2(15, 15));
	ImGui::Begin(name.c_str(), nullptr, ImGuiWindowFlags_NoResize);


	
	ImGui::PushStyleColor(ImGuiCol_Button,         (ImVec4)ImColor::ImColor(0.25f, 0.55f, 0.9f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered,  (ImVec4)ImColor::ImColor(0.0f,  0.45f,  0.9f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive,   (ImVec4)ImColor::ImColor(0.25f, 0.25f, 0.9f));	
	if (ImGui::Button("Duplicate"))
	{

	}
	ImGui::PopStyleColor(3);
	ImGui::SameLine();
	if (ImGui::Button("Delete"))
	{

	}

	// if object - > transform
	ImGui_Transform();

	// if object - > component
	// ImGui_Component(&component);

	ImGui::End();
}


void ImGui_Transform()
{
	if (ImGui::CollapsingHeader("Transform Component"))
	{
		static float x, y, z;
		static float sx, sy, rotation;
		if (ImGui::CollapsingHeader("Position"))
		{
			ImGui::PushItemWidth(120);
			ImGui::InputFloat(" X", &x);
			ImGui::InputFloat(" Y", &y);
			ImGui::InputFloat(" Z", &z);
			ImGui::PopItemWidth();			
		}
		if (ImGui::CollapsingHeader("Scale"))
		{
			ImGui::PushItemWidth(120);
			ImGui::InputFloat(" X", &sx);
			ImGui::InputFloat(" Y", &sy);
			ImGui::PopItemWidth();
			ImGui::Separator();
		}
		
		ImGui::InputFloat("Rotation", &rotation);
	}
}



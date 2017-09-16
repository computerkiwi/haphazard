

#include "Editor.h"

#include <string>
#include <sstream>


#include "GameObjectSystem\GameSpace.h"
#include "GameObjectSystem\TextSprite.h"
#include "GameObjectSystem\TransformComponent.h"

#include "Util\Logging.h"

#include "Engine\Engine.h"

void TopBar();
void Console();
void ImGui_GameObject(GameObject *object);
void ImGui_Transform(TransformComponent *transform);
void ImGui_TextSprite(TextSprite *tsprite);

// Enter Hex code not including the 0x
#define HexVec(HEX) (static_cast<float>(0xFF000000 & HEX) / 0xFF000000), \
	                (static_cast<float>(0x00FF0000 & HEX) / 0x00FF0000), \
	                (static_cast<float>(0x0000FF00 & HEX) / 0x0000FF00), \
	                (static_cast<float>(0x000000FF & HEX) / 0x000000FF)

// ((0xFF000000 & HEX) / 0xFF000000)


void Editor_Init()
{
	// Style information
	ImGuiStyle * style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	style->Colors[ImGuiCol_Text]         = ImVec4(HexVec(0xCCCCD3FF));
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);

	style->Colors[ImGuiCol_WindowBg]      = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	
	style->Colors[ImGuiCol_Border]       = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	
	style->Colors[ImGuiCol_FrameBg]        = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive]  = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	
	style->Colors[ImGuiCol_TitleBg]          = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive]    = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	
	style->Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	
	style->Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
	
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	
	style->Colors[ImGuiCol_SliderGrab]       = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	
	style->Colors[ImGuiCol_Button]        = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive]  = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	
	style->Colors[ImGuiCol_Header]        = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive]  = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	
	style->Colors[ImGuiCol_Column]        = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ColumnActive]  = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	
	style->Colors[ImGuiCol_ResizeGrip]        = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive]  = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	
	style->Colors[ImGuiCol_CloseButton]        = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
	style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
	style->Colors[ImGuiCol_CloseButtonActive]  = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
	
	style->Colors[ImGuiCol_PlotLines]            = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram]        = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	
	style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
}


void Editor(Engine *engine)
{
	// TopBar();

	auto objects = engine->GetSpace()->CollectGameObjects();
	for (auto& object : objects)
	{
		ImGui_GameObject(&object);
	}
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

		ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor(0.25f, 0.55f, 0.9f)));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor(0.0f, 0.45f, 0.9f)));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor(0.25f, 0.25f, 0.9f)));
		if (ImGui::Button("Duplicate"))
		{
			object->Duplicate<dummy>();
		}
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			object->Delete<dummy>();
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
		if (ImGui::CollapsingHeader("Position"))
		{
			ImGui::PushItemWidth(120);
			ImGui::InputFloat(" X", &transform->GetPosition().x);
			ImGui::InputFloat(" Y", &transform->GetPosition().y);
			ImGui::PopItemWidth();
		}
		if (ImGui::CollapsingHeader("Scale"))
		{
			ImGui::PushItemWidth(120);
			ImGui::InputFloat(" X ", &transform->GetScale().x);
			ImGui::InputFloat(" Y ", &transform->GetScale().y);
			ImGui::PopItemWidth();
			ImGui::Separator();
		}

		ImGui::PushItemWidth(120);
		ImGui::SliderFloat("", &transform->GetRotation(), 0, 360); 
		ImGui::SameLine();
		ImGui::PopItemWidth();
		ImGui::PushItemWidth(100);
		ImGui::InputFloat("Rotation", &transform->GetRotation(), 0.0f, 0.0f, 2); 
		ImGui::PopItemWidth();
	}
}


void ImGui_TextSprite(TextSprite *tsprite)
{
	if (ImGui::CollapsingHeader("Text Sprite"))
	{
		ImGui::Text(tsprite->m_name);
		ImGui::Separator();
	}
}


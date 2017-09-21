/*
FILE: Editor.cpp
PRIMARY AUTHOR: Sweet

Copyright © 2017 DigiPen (USA) Corporation.
*/

// http://www.ariel.com/images/animated-messages-small2.gif

#include "Editor.h"

#include <string>
#include <sstream>
#include <algorithm>

#include "../Imgui/imgui-setup.h"

#include "GameObjectSystem\GameSpace.h"
#include "GameObjectSystem\TransformComponent.h"

#include "Util\Logging.h"

#include "Engine\Engine.h"


#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <locale>
#include <codecvt>


void ImGui_GameObject(GameObject *object);
void ImGui_Transform(TransformComponent *transform);

// Enter Hex code not including the 0x
#define HexVec(HEX) (static_cast<float>(0xFF000000 & HEX) / 0xFF000000), \
	                (static_cast<float>(0x00FF0000 & HEX) / 0x00FF0000), \
	                (static_cast<float>(0x0000FF00 & HEX) / 0x0000FF00), \
	                (static_cast<float>(0x000000FF & HEX) / 0x000000FF)

Editor::Editor(Engine *engine, GLFWwindow *window) : m_objects(), m_engine(engine)
{
	// Style information
	ImGuiStyle * style = &ImGui::GetStyle();

	ImGui_ImplGlfwGL3_Init(window, true);

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

	RegisterCommand("log", []() { Logging::Log("Hello!"); });
	RegisterCommand("exit", []() { std::exit(0); });

}

Editor::~Editor()
{
	ImGui_ImplGlfwGL3_Shutdown();
}


void Editor::Update()
{
	ImGui_ImplGlfwGL3_NewFrame();
	ImGui::ShowTestWindow();
	m_objects = m_engine->GetSpace()->CollectGameObjects();
	Console();
	ImGui_GameObject(&m_objects[0]);

	ImGui::Render();
}


void Editor::RegisterCommand(const char *command, std::function<void()>&& f)
{
	m_commands.emplace(command, f);
}


void Editor::Log(const char *log_message, ...)
{
	int old_size = m_log_buffer.size();
	std::stringstream ss;

	auto t = std::time(nullptr);
	
	#pragma warning(disable : 4996)
	ss << std::put_time(std::localtime(&t), "[%H:%M:%S]") << " - " << log_message << std::endl;

	va_list args;
	va_start(args, log_message);
	m_log_buffer.appendv(ss.str().c_str(), args);
	va_end(args);
	m_offsets.push_back(m_log_buffer.size() - 1);
}


void Editor::SetActive(ImGuiTextEditCallbackData* data, int entryIndex)
{
	memmove(data->Buf, m_log_history[entryIndex].c_str(), m_log_history[entryIndex].length());
	data->BufDirty = true;
	data->BufTextLen = static_cast<int>(m_log_history[entryIndex].length());
}


int Input(ImGuiTextEditCallbackData *data)
{
	Editor *editor = reinterpret_cast<Editor *>(data->UserData);

	switch (data->EventFlag)
	{
	case ImGuiInputTextFlags_CallbackCompletion:
			if (editor->m_state.m_popUp && editor->m_state.activeIndex != -1)
			{

			}
			editor->m_state.m_popUp = false;
			editor->m_state.activeIndex = -1;
			editor->m_state.clickedIndex = -1;
		
		break;

	case ImGuiInputTextFlags_CallbackHistory:
		editor->m_state.m_popUp = true;

			if (data->EventKey == ImGuiKey_UpArrow && editor->m_state.activeIndex > 0)
			{
				editor->m_state.activeIndex--;
				editor->m_state.m_selection_change = true;
			}
			else if (data->EventKey == ImGuiKey_DownArrow && editor->m_state.activeIndex < editor->m_log_history.size() - 1)
			{
				editor->m_state.activeIndex++;
				editor->m_state.m_selection_change = true;
			}

		break;

	case ImGuiInputTextFlags_CallbackAlways:
			if (editor->m_state.activeIndex != -1)
			{
				
				editor->m_state.activeIndex = -1;
				editor->m_state.clickedIndex = -1;
				editor->m_state.m_popUp = false;
			}
		break;
	case ImGuiInputTextFlags_CallbackCharFilter:
		break;
	}

	return 0;
}


bool Editor::PopUp(ImVec2& pos, ImVec2& size)
{
	if (!m_state.m_popUp)
		return false;

	ImGuiWindowFlags flags = 
        ImGuiWindowFlags_NoTitleBar          | 
        ImGuiWindowFlags_NoResize            |
        ImGuiWindowFlags_NoMove              |
        ImGuiWindowFlags_HorizontalScrollbar |
        ImGuiWindowFlags_NoSavedSettings     |
        ImGuiWindowFlags_ShowBorders;
	ImGui::SetNextWindowPos(pos);
	ImGui::SetNextWindowSize(size);
	ImGui::Begin("console_popup", nullptr, flags);
	
	ImGui::PushAllowKeyboardFocus(false);

	for (auto i = 0; i < m_log_history.size(); ++i)
	{
		bool isActiveIndex = m_state.activeIndex == i;
		if (isActiveIndex)
		{
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1, 0, 0, 1));
		}

		ImGui::PushID(i);
		if (ImGui::Selectable(m_log_history[i].c_str(), m_state.activeIndex))
		{
			m_state.clickedIndex = i;
		}
		ImGui::PopID();

		if (isActiveIndex)
		{
			if (m_state.m_selection_change)
			{
				ImGui::SetScrollHere();
				m_state.m_selection_change = false;
			}

			ImGui::PopStyleColor(1);
		}
	}

	bool isFocuesed = ImGui::IsRootWindowFocused();

	ImGui::PopAllowKeyboardFocus();
	ImGui::End();

	return isFocuesed;
}


void Editor::Console()
{
	char command_buffer[1024] = { 0 };
	ImGui::SetNextWindowPosCenter(ImGuiSetCond_FirstUseEver);

	auto winflags = 0;
	if (m_state.m_popUp)
		winflags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("Console", nullptr, winflags);

	if (ImGui::Button("Clear Log"))
	{
		Clear();
	}

	ImGui::SameLine();
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));
	if (ImGui::Button("Copy"))
	{
		ImGui::LogToClipboard();
	}
	ImGui::PopStyleVar();
	
	ImGui::SameLine();
	m_log_filter.Draw("Filter", -100.0f);

	ImGui::BeginChild("scrolling", ImVec2(470, 285));
	if (m_log_filter.IsActive())
	{
		const char* buf_begin = m_log_buffer.begin();
		const char* line = buf_begin;

		for (int line_no = 0; line != NULL; line_no++)
		{
			const char* line_end = (line_no < m_offsets.Size) ? buf_begin + m_offsets[line_no] : NULL;

			if (m_log_filter.PassFilter(line, line_end))
			{
				ImGui::TextUnformatted(line, line_end);
			}

			line = line_end && line_end[1] ? line_end + 1 : NULL;
		}
	}
	else
	{
		ImGui::TextUnformatted(m_log_buffer.begin());
	}
	ImGui::EndChild();
	ImGui::Separator();



	ImGui::Text("Command: ");
	ImGui::SameLine();

	auto flags = ImGuiInputTextFlags_CtrlEnterForNewLine  |
				 ImGuiInputTextFlags_EnterReturnsTrue     |
				 ImGuiWindowFlags_NoSavedSettings         |
				 ImGuiInputTextFlags_CallbackAlways       |
				 ImGuiInputTextFlags_CallbackCharFilter   |
				 ImGuiInputTextFlags_CallbackCompletion   |
				 ImGuiInputTextFlags_CallbackHistory;

	if (ImGui::InputText("", command_buffer, 300, flags, Input, this))
	{
		std::string command(command_buffer);
		std::transform(command.begin(), command.end(), command.begin(), ::tolower);

		if (m_log_history.size() < 8092)
		{
			m_log_history.emplace_back(command.c_str());
		}

		
		this->Log(command.c_str());

		if (m_state.m_popUp && m_state.activeIndex != -1)
		{
			memmove(command_buffer, m_log_history[m_state.activeIndex].c_str(), m_log_history[m_state.activeIndex].size());
		}
		else
		{
			for (auto& cmd : m_commands)
			{
				if (cmd.first == command)
					cmd.second();
			}
		}
		m_state.m_popUp = false;
		m_state.activeIndex = -1;
		ImGui::SetKeyboardFocusHere(-1);
	}

	if (m_state.clickedIndex != -1)
	{
		ImGui::SetKeyboardFocusHere(-1);
		m_state.m_popUp = false;
	}

	// Draw PopUp
	ImVec2 pop_pos(ImGui::GetItemRectMin());
	ImVec2 pop_size(ImGui::GetItemRectSize().x - 60, ImGui::GetItemsLineHeightWithSpacing() * 4);
	bool windowFocus = ImGui::IsRootWindowFocused();

	pop_pos.y += ImGui::GetItemRectSize().y;

	bool popUpFocus = PopUp(pop_pos, pop_size);

	if (!windowFocus && !popUpFocus)
	{
		m_state.m_popUp = false;
	}

	ImGui::End();
}


void Editor::Clear()
{
	m_log_buffer.clear();
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
		if (object->GetComponent<TransformComponent>().Get())
		{
			ImGui_Transform(object->GetComponent<TransformComponent>().Get());
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
			ImGui::InputFloat(" X", &transform->Position().x);
			ImGui::InputFloat(" Y", &transform->Position().y);
			ImGui::PopItemWidth();
		}
		if (ImGui::CollapsingHeader("Scale"))
		{
			ImGui::PushItemWidth(120);
			ImGui::InputFloat(" X ", &transform->Scale().x);
			ImGui::InputFloat(" Y ", &transform->Scale().y);
			ImGui::PopItemWidth();
			ImGui::Separator();
		}

		ImGui::PushItemWidth(120);
		ImGui::SliderFloat("", &transform->Rotation(), 0, 360); 
		ImGui::SameLine();
		ImGui::PopItemWidth();
		ImGui::PushItemWidth(100);
		ImGui::InputFloat("Rotation", &transform->Rotation(), 0.0f, 0.0f, 2); 
		ImGui::PopItemWidth();
	}
}


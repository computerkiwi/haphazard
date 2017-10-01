/*
FILE: Editor.cpp
PRIMARY AUTHOR: Sweet

Copyright � 2017 DigiPen (USA) Corporation.
*/

// http://www.ariel.com/images/animated-messages-small2.gif

#include "Editor.h"
#include "Type_Binds.h"

#include <string>
#include <sstream>
#include <algorithm>

#include "../Imgui/imgui-setup.h"

#include "GameObjectSystem\GameSpace.h"
#include "GameObjectSystem/GameObject.h"
#include "Engine\Physics\RigidBody.h"
#include "graphics\SpriteComponent.h"
#include "Engine\Physics\Collider2D.h"

#include "Util\Logging.h"

#include "Engine\Engine.h"

#include "Input\Input.h"

#include <iomanip>
#include <locale>
#include <ctype.h>



Editor::Editor(Engine *engine, GLFWwindow *window) : m_engine(engine), m_show_editor(true), m_objects(), m_state{ false, -1, -1, false }
{
	// Style information
	ImGuiStyle * style = &ImGui::GetStyle();

	// Setup the imgui data
	ImGui_ImplGlfwGL3_Init(window, true);

	// Window styles
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

	// Colors for all the different items
	style->Colors[ImGuiCol_Text]         = ImVec4(HexVecA(0xCCCCD3FF));
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


	auto help = [this]()
	{
		Editor::Internal_Log("    Commands Avaiable: \n");
		
		for (auto& cmd : m_commands)
		{
			Editor::Internal_Log("     - %s \n", cmd.command);
		}
	};

	// Pre Allocate the Command Data
	m_commands.reserve(32);
	
	// Help screen bs
	RegisterCommand("?", help);
	RegisterCommand("help", help);

	// Create gameobject
	RegisterCommand("create", [this]() { CreateGameObject(); });
	
	// Clear the log
	RegisterCommand("clear", [this]() { Clear(); });
	RegisterCommand("cls", [this]() { Clear(); });

	// Exit the game, probably should be editor?
	RegisterCommand("exit", [this]() { m_engine->Exit(); });

	// Display past commands
	RegisterCommand("history",
	[this]()
	{
		Editor::Internal_Log("    Command History: \n");

		for (auto& history : m_log_history)
		{
			Editor::Internal_Log("     - %s \n", history);
		}
	});

	// Log something to the file
	RegisterCommand("log", 
	[this]()
	{
		Logging::Log(m_line.substr(strlen("log")).c_str());
	});

	// Display the current active objects
	RegisterCommand("objects",
	[this]()
	{
		Editor::Internal_Log("    Current Objects: \n");
		for (auto object : m_objects)
		{
			Editor::Internal_Log("     - %d \n", object);
		}
	});

	// Select an object
	RegisterCommand("select",
	[this]()
	{
		int seleted_id = atoi(m_line.substr(strlen("select ")).c_str());

		for (auto object : m_objects)
		{
			if (object == seleted_id)
			{
				m_selected_object = object;
				break;
			}
		}
	});
}


Editor::~Editor()
{
	ImGui_ImplGlfwGL3_Shutdown();
}


void Editor::Update()
{
	// Check if Editor is being shown
	if (m_show_editor)
	{
		// Check for click events
		OnClick();

		// Start a new frame in imgui
		ImGui_ImplGlfwGL3_NewFrame();

		// Get all the active gameobjects
		m_objects = m_engine->GetSpace(GameObject(m_selected_object).GetIndex())->CollectGameObjects();

		// Render the console
		Console();

		//ImGui::ShowTestWindow();

		// Display
		ObjectsList();

		// Pass the current object in the editor
		ImGui_GameObject(GameObject(m_selected_object));

		ImGui::Render();
	}
}

// Register a command using a lambda
void Editor::RegisterCommand(const char *command, std::function<void()>&& f)
{
	m_commands.emplace_back(Command(command, strlen(command), f));
}

// External Log that displays date
void Editor::Log(const char *log_message, ...)
{
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


// Log Data, works like printf
// This internal log doesnt show datetime
void Editor::Internal_Log(const char * log_message, ...)
{
	// Add to the log buffer
	va_list args;
	va_start(args, log_message);
	m_log_buffer.appendv(log_message, args);
	va_end(args);
	m_offsets.push_back(m_log_buffer.size() - 1);
}



void Editor::CreateGameObject(glm::vec2& pos, glm::vec2& size)
{
	GameObject_ID object = m_engine->GetSpace(GameObject(m_selected_object).GetIndex())->NewGameObject();
	GameObject(object).AddComponent<TransformComponent>(glm::vec3(pos.x, pos.y, 1.0f), glm::vec3(size.x, size.y, 1.0f));
	m_selected_object = object;
}


void Editor::SetGameObject(GameObject_ID new_object)
{
	m_selected_object = new_object;
}


void Editor::OnClick()
{
	// Check for mouse 1 click
	if (Input::IsPressed(Key::MOUSE_1) && !ImGui::GetIO().WantCaptureMouse)
	{
		const glm::vec2& mouse = Input::GetMousePos();

		for (auto& transform : *m_engine->GetSpace(GameObject(m_selected_object).GetIndex())->GetComponentMap<TransformComponent>())
		{
			const glm::vec3& scale = transform.Get()->Scale();
			const glm::vec3& pos = transform.Get()->Position();

			if (mouse.x < pos.x + scale.x && mouse.x > pos.x - scale.x)
			{
				if (mouse.y < pos.y + scale.y && mouse.y > pos.y - scale.y)
				{
					// Save the GameObject data
					m_selected_object = transform.GetGameObject_ID();
				}
			}
			else if (mouse.x < pos.x - scale.x)
			{
				if (mouse.y > pos.y + scale.y)
				{
					m_selected_object = transform.GetGameObject_ID();
				}
				else if (mouse.y < pos.y - scale.y)
				{
					m_selected_object = transform.GetGameObject_ID();
				}
			}
		}
	}
}


void Editor::ObjectsList()
{
	using namespace ImGui;

	SetNextWindowSize(ImVec2(225, 350));
	Begin("Objects", nullptr, ImGuiWindowFlags_NoSavedSettings);

	// Get all the names of the objects
	std::string name("GameObject - ");
	for (auto& object : m_objects)
	{
		name += std::to_string(GameObject(object).Getid());
		if (Selectable(name.c_str()))
		{
			SetGameObject(object);
			break;
		}

		name.clear();
		name = "GameObject - ";
	}

	End();
}


void Editor::SetActive(ImGuiTextEditCallbackData *data, int entryIndex)
{
	// Copy in the data  from the command
	memmove(data->Buf, m_commands[entryIndex].command, m_commands[entryIndex].cmd_length);

	// Update the Buffer data
	data->Buf[m_commands[entryIndex].cmd_length] = '\0';
	data->BufTextLen = static_cast<int>(m_commands[entryIndex].cmd_length);
	data->BufDirty = true;
}


bool Command_StrCmp(const char *str1, const char *str2)
{
	while (!(*str1 ^ *str2++))
		if (!*str1++)
			return true;

	return false;
}


static int   Strnicmp(const char* str1, const char* str2, int n) 
{ 
	int d = 0; 
	while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) 
	{ 
		str1++; 
		str2++;
		n--; 
	} 
	return d; 
}



int Input_Editor(ImGuiTextEditCallbackData *data)
{
	Editor *editor = reinterpret_cast<Editor *>(data->UserData);

	switch (data->EventFlag)
	{
	case ImGuiInputTextFlags_CallbackCompletion:
		
		break;

		// History based data
	case ImGuiInputTextFlags_CallbackHistory:
			editor->m_state.m_popUp = true;

			if (data->EventKey == ImGuiKey_UpArrow && editor->m_state.activeIndex > 0)
			{
				editor->m_state.activeIndex--;
				editor->m_state.m_selection_change = true;
			}
			else if (data->EventKey == ImGuiKey_DownArrow && (editor->m_state.activeIndex < static_cast<int>(editor->m_commands.size() - 1)))
			{
				editor->m_state.activeIndex++;
				editor->m_state.m_selection_change = true;
			}

		break;

	case ImGuiInputTextFlags_CallbackAlways:
			// Clear the data in the matches vector, but dont free the alloc'd memory
			editor->m_matches.clear_nofree();

			// Make sure there is data to read from
			if (data->Buf && data->BufTextLen > 0)
			{
				// If there is data then the popup needs to be drawn
				editor->m_state.m_popUp = true;

				// Get the word from the data
				const char *word_end = data->Buf + data->CursorPos;
				const char *word_start = word_end;

				// Find the beginning and end of the word
				while (word_start > data->Buf)
				{
					// Start at the last letter
					const char c = word_start[-1];

					// End of word
					if (c == ' ' || c == '\t' || c == ',' || c == ';')
						break;

					// Move back a letter
					--word_start;
				}

				// Check if the input matches any commands
				for (int i = 0; i < editor->m_commands.size(); ++i)
				{
					// If there is some amount of match then add it to the vector
					if (Strnicmp(editor->m_commands[i].command, word_start, (int)(word_end - word_start)) == 0)
						editor->m_matches.push_back(editor->m_commands[i].command);
				}

				// Check if there were matches
				if (editor->m_matches.Size)
				{
					// Get the length of the match
					int length = static_cast<int>(word_end - word_start);

					// Check for length on matches
					while (true)
					{
						char character = 0;
						bool all_matches_made = true;

						for (int i = 0; i < editor->m_matches.Size && all_matches_made; ++i)
						{
							if (i == 0)
							{
								character = editor->m_matches[i][length];
							}
							else if (character == 0 || character != editor->m_matches[i][length])
							{
								all_matches_made = false;
							}
						}

						if (!all_matches_made)
							break;
						length++;
					}

					if (length > 0)
					{
						data->DeleteChars(static_cast<int>(word_start - data->Buf), static_cast<int>(word_end - word_start));
						data->InsertChars(data->CursorPos, editor->m_matches[0], editor->m_matches[0] + length);
					}
				}
			}
			if (editor->m_state.clickedIndex != -1)
			{
				editor->SetActive(data, editor->m_state.clickedIndex);
				editor->m_state.activeIndex = -1;
				editor->m_state.clickedIndex = -1;
				editor->m_state.m_popUp = false;
			}
		break;
	case ImGuiInputTextFlags_CallbackCharFilter:
		break;

	default:
		break;
	}

	return 0;
}


bool Editor::PopUp(ImVec2& pos, ImVec2& size)
{
	// Make sure the popup needs to be seen
	if (!m_state.m_popUp || !m_matches.size())
		return false;

	ImGuiWindowFlags flags = 
        ImGuiWindowFlags_NoTitleBar          | 
        ImGuiWindowFlags_NoResize            |
        ImGuiWindowFlags_NoMove              |
        ImGuiWindowFlags_HorizontalScrollbar |
        ImGuiWindowFlags_NoSavedSettings     |
        ImGuiWindowFlags_ShowBorders;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);

	// Popup begin
	ImGui::SetNextWindowPos(pos);
	ImGui::SetNextWindowSize(size);
	ImGui::Begin("console_popup", nullptr, flags);
	
	ImGui::PushAllowKeyboardFocus(false);

	// Go through the matches found from the input text
	for (auto i = 0; i < m_matches.size(); ++i)
	{
		// Check if the item is the hovered or active for colors
		bool isActiveIndex = m_state.activeIndex == i;
		if (isActiveIndex)
		{
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(HexVecA(0x072f70FF)));
		}
		
		// Display the options as a selectable text button
		ImGui::PushID(i);
		if (ImGui::Selectable(m_matches[i], m_state.activeIndex))
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
	ImGui::PopStyleVar(1);
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

	// The log box itself
	ImGui::BeginChild("scrolling", ImVec2(470, 285));

	// Check if any filters are active and need to be applied
	if (m_log_filter.IsActive())
	{
		// Get the start of the data
		const char *buf_begin = m_log_buffer.begin();
		const char *line = buf_begin;

		// Go through the data and 
		for (int line_no = 0; line != NULL; line_no++)
		{
			// Get the data
			const char *line_end = (line_no < m_offsets.Size) ? buf_begin + m_offsets[line_no] : NULL;

			// Filter the text
			if (m_log_filter.PassFilter(line, line_end))
			{
				ImGui::TextUnformatted(line, line_end);
			}

			line = line_end && line_end[1] ? line_end + 1 : NULL;
		}
	}
	else
	{
		// Put the text out without a filter
		ImGui::TextUnformatted(m_log_buffer.begin());
	}
	ImGui::EndChild();
	ImGui::Separator();



	ImGui::Text("Command: ");
	ImGui::SameLine();

	auto flags = ImGuiInputTextFlags_EnterReturnsTrue     |
				 ImGuiWindowFlags_NoSavedSettings         |
				 ImGuiInputTextFlags_CallbackAlways       |
				 ImGuiInputTextFlags_CallbackCharFilter   |
				 ImGuiInputTextFlags_CallbackCompletion   |
				 ImGuiInputTextFlags_CallbackHistory;

	if (ImGui::InputText("", command_buffer, 512, flags, Input_Editor, this))
	{
		// Get the line data and save it for parameters
		m_line = command_buffer;

		// Make sure there is data in the line
		if (m_line.size() > 0)
		{
			// Skip leading spaces
			auto first_of_not_space = m_line.find_first_not_of(' ');

			std::string command = m_line.substr(first_of_not_space, m_line.find_first_of(' '));

			// Make Everything uppercase
			std::transform(command.begin(), command.end(), command.begin(), ::tolower);

			// Log the command and display it on screen
			if (first_of_not_space != std::string::npos)
			{
				m_line = m_line.substr(first_of_not_space);
				this->Log(m_line.c_str());

				// Save the command for history
				if (m_log_history.size() < 100)
				{
					m_log_history.emplace_back(m_line.c_str());
				}
			}

			// Check if the popup was clicked and copy the data from the clicked item to here
			if (m_state.m_popUp && m_state.clickedIndex != -1)
			{
				memmove(command_buffer, m_commands[m_state.activeIndex].command, m_commands[m_state.activeIndex].cmd_length);
			}
			else
			{
				// Parse the command
				for (auto& cmd : m_commands)
				{
					if (cmd.command == command)
					{
						cmd.func();
						break;
					}
				}
			}

			// Reset keyboard focus to the console input and reset the state of the popup
			m_state.m_popUp = false;
			m_state.activeIndex = -1;
			ImGui::SetKeyboardFocusHere(-1);
		}
	}

	if (m_state.clickedIndex != -1)
	{
		ImGui::SetKeyboardFocusHere(-1);
		m_state.m_popUp = false;
	}

	if ((ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
		ImGui::SetKeyboardFocusHere(-1);

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
	m_log_buffer.clear_nofree();
}


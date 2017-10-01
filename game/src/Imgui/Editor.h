/*
FILE: Editor.h
PRIMARY AUTHOR: Sweet

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include <imgui.h>
#include <vector>
#include <map>
#include <functional>
#include <glm/detail/type_vec2.hpp>


typedef std::size_t GameObject_ID;

class Engine;
struct GLFWwindow;

// These Macros allow for hex color codes
// Enter Hex code not including the 0x
#define HexVecA(HEX) (static_cast<float>(0xFF000000 & HEX) / 0xFF000000), \
	                 (static_cast<float>(0x00FF0000 & HEX) / 0x00FF0000), \
	                 (static_cast<float>(0x0000FF00 & HEX) / 0x0000FF00), \
	                 (static_cast<float>(0x000000FF & HEX) / 0x000000FF)

#define HexVec(HEX) (static_cast<float>(0xFF000000 & HEX) / 0xFF000000), \
	                (static_cast<float>(0x00FF0000 & HEX) / 0x00FF0000), \
	                (static_cast<float>(0x0000FF00 & HEX) / 0x0000FF00)


class Editor
{
	Engine * m_engine;
	bool m_show_editor;

	GameObject_ID m_selected_object = 0;
	std::vector<GameObject_ID> m_objects;


	std::string m_line;

	friend bool Command_StrCmp(const char *str1, const char *str2);
	struct Command
	{
		Command() : command(nullptr), cmd_length(0), func(std::function<void()>()) {}
		Command(const char *cmd, size_t len, std::function<void()> f) : command(cmd), cmd_length(len), func(f) {}

		const char * command = nullptr;
		size_t cmd_length = 0;
		std::function<void()> func = std::function<void()>();
	};

	std::vector<Command> m_commands;
	std::vector<const char *> m_log_history;
	ImVector<const char *> m_matches;

	void SetActive(ImGuiTextEditCallbackData* data, int entryIndex);
	struct State 
	{
		bool m_popUp;
		int activeIndex;
		int clickedIndex;
		bool m_selection_change;
	} m_state;
	ImGuiTextBuffer m_log_buffer;
	ImGuiTextFilter m_log_filter;
	ImVector<int>   m_offsets;

	

private:
	friend int Input_Editor(ImGuiTextEditCallbackData *data);
	bool PopUp(ImVec2& pos, ImVec2& size);
	void CreateGameObject(glm::vec2& pos = glm::vec2(0, 0), glm::vec2& size = glm::vec2(1, 1));
	void ObjectsList();

	void Internal_Log(const char *log_message, ...);

	void OnClick();

public:
	Editor(Engine *engine, GLFWwindow *window);
	~Editor();

	void Update();

	// Works like printf -- for display_date use true
	void Log(const char *log_message, ...);
	void Clear();

	void SetGameObject(GameObject_ID new_object);
	

	void Console();
	void RegisterCommand(const char *command, std::function<void()>&& f);
};


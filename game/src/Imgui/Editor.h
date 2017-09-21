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

class Engine;
class GameObject;
struct GLFWwindow;

class Editor
{
	std::map<std::string, std::function<void()>> m_commands;

	std::vector<GameObject> m_objects;
	Engine * m_engine;

	std::vector<std::string> m_log_history;
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

	friend int Input(ImGuiTextEditCallbackData *data);
	bool PopUp(ImVec2& pos, ImVec2& size);

public:
	Editor(Engine *engine, GLFWwindow *window);
	~Editor();

	void Log(const char *log_message, ...);
	void Clear();

	void Console();
	void Update();
	void RegisterCommand(const char *command, std::function<void()>&& f);
};


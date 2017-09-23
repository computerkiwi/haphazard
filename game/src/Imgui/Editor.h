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
#include "GameObjectSystem/GameObject.h"
#include <glm/detail/type_vec2.hpp>


class Engine;
struct GLFWwindow;

class Editor
{
	std::map<std::string, std::function<void()>> m_commands;

	GameObject m_selected_object = GameObject(0, nullptr);
	std::vector<GameObject> m_objects;
	Engine * m_engine;

	std::string m_line;

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
	void CreateGameObject(glm::vec2& pos = glm::vec2(0, 0), glm::vec2& size = glm::vec2(1, 1));
public:
	Editor(Engine *engine, GLFWwindow *window);
	~Editor();

	void Log(const char *log_message, ...);
	void Clear();

	void SetGameObject(GameObject& new_object);

	void Console();
	void Update();
	void RegisterCommand(const char *command, std::function<void()>&& f);
};


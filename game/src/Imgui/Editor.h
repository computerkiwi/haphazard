/*
          FILE: Editor.h
PRIMARY AUTHOR: Sweet

Copyright � 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include <imgui.h>
#include <vector>
#include <map>
#include <functional>
#include <glm/detail/type_vec2.hpp>

#include "GameObjectSystem\GameObject.h"
#include "Util/DataStructures/Array/Array.h"

#include "meta\meta.h"

class Engine;
class TransformComponent;
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

#define MAX_SELECT 10

struct EditorAction;
typedef void(*actionFunc)(EditorAction& a);

struct EditorAction
{
	meta::Any old_value;
	meta::Any new_value;
	void *object;
	actionFunc func;

	bool redo;
};


enum PopUpPosition
{
	TopLeft,
	TopRight,
	BottomRight,
	BottomLeft,
	Center, 
	Mouse
};
struct PopUpWindow
{
	PopUpWindow(const char *msg, float time, PopUpPosition position) 
		: message(msg), timer(time), pos(position) {}
	const char *message;
	float timer;
	PopUpPosition pos;
};


class Editor
{
	friend void ImGui_Transform(TransformComponent *transform, GameObject object, Editor *editor);
	friend void Choose_Parent_ObjectList(Editor *editor, TransformComponent *transform, GameObject child);

	Engine * m_engine;
	bool m_show_editor;

	int m_current_space_index = 0;
	GameObject_ID m_selected_object = 0;
	Array<GameObject_ID, MAX_SELECT> m_multiselect;

	struct Actions
	{
		std::vector<EditorAction> history;
		size_t size = 0;
	} m_actions;
	std::vector<GameObject_ID> m_objects;

	enum Tool
	{
		none,
		Translation,
		Scale,
		Rotation
	};
	Tool m_tool = none;

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

	bool m_show_console = false;
	bool m_scroll = false;
	std::map<std::size_t, Command> m_commands;
	std::vector<std::string> m_log_history;
	ImVector<const char *> m_matches;

	void SetActive_Completion(ImGuiTextEditCallbackData *data, int entryIndex);
	void SetActive_History(ImGuiTextEditCallbackData *data, int entryIndex);
	void SetActive(ImGuiTextEditCallbackData *data, size_t entryIndex);
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

	void UpdatePopUps(float dt);
	std::vector<PopUpWindow> m_pop_ups;

private:
	friend int Input_Editor(ImGuiTextEditCallbackData *data);
	bool PopUp(ImVec2& pos, ImVec2& size);
	void QuickCreateGameObject(const char *name, glm::vec2& pos = glm::vec2(0, 0), glm::vec2& size = glm::vec2(1, 1));
	void ObjectsList();


	void OnClick();

public:
	Editor(Engine *engine, GLFWwindow *window);
	~Editor();

	void Update();

	// Works like printf -- for display_date use true
	void Log(const char *log_message, ...);
	
	// No timestamp
	void Internal_Log(const char *log_message, ...);
	void Clear();

	void Push_Action(EditorAction&& a);
	void Undo_Action();
	void Redo_Action();

	void AddPopUp(PopUpWindow&& pop);

	void SetGameObject(GameObject new_object);
	void PrintObjects();
	void ToggleEditor();

	void Tools();

	void MenuBar();
	void Console();
	void RegisterCommand(const char *command, std::function<void()>&& f);
};

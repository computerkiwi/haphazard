/*
FILE: Editor.cpp
PRIMARY AUTHOR: Sweet

Copyright (c) 2017 DigiPen (USA) Corporation.
*/

// http://www.ariel.com/images/animated-messages-small2.gif

// Main Editor Includes
#include "Editor.h"
#include "Type_Binds.h"
#include "MultiselectUndo.h"

// Standard Includes
#include "Engine/Engine.h"
#include "Util/Logging.h"
#include "graphics\Settings.h"

// ImGui Init, NewFrame, Shutdown Calls
#include "../Imgui/imgui-setup.h"

// GameSpace Class
#include "GameObjectSystem/GameSpace.h"

// GameObject Class
#include "GameObjectSystem/GameObject.h"

// Components
#include "GameObjectSystem/ObjectInfo.h"
#include "Engine/Physics/RigidBody.h"
#include "graphics/SpriteComponent.h"
#include "graphics/Particles.h"
#include "Engine/Physics/Collider2D.h"
#include "Scripting/ScriptComponent.h"
#include "graphics/Camera.h"
#include "graphics/Background.h"
#include "Engine/Physics/Raycast.h"

// Key and Mouse Input
#include "Input/Input.h"

// Debug Graphics for Shapes
#include "graphics/DebugGraphic.h"

// Access point for reloading all the scripts.
#include "Scripting/ScriptSystem.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <iomanip>
#include <algorithm>
#include <locale>
#include <ctype.h>

#ifdef _WIN32
	#include <Windows.h>
	#include <psapi.h>
#endif

// Define PI since I couldn't get the cmath one to want to work -- sigh
#define PI 3.1415926535f

// Since actual dt isn't that important
#define FAKE_DT (1 / 60.0f)

// Set the initial global scale
float Editor::globalScale = 1.0f;

// Gizmo Transform Save Location
TransformComponent objectSave;

// General Action for undo/redo
template <class Component, typename T>
void Action_General(EditorAction& a)
{
	// Get the component handle for the object
	ComponentHandle<Component> handle(a.handle);

	// Get the Component Object
	meta::Any obj(handle.Get());

	if (handle.GetGameObject().IsValid())
	{
		// Check if we want redo or undo
		if (a.redo)
		{
			// Update the data to the new value (redo)
			obj.SetPointerMember(a.name, a.current.GetData<T>());
		}
		else
		{
			// Update the data with the old value (undo)
			obj.SetPointerMember(a.name, a.save.GetData<T>());
		}
	}
	else
	{
		logger << "[EDITOR] Attempting to undo/redo on a deleted object. Undo for deleted objects is coming soon.\n";
	}
}


void Action_AddComponent_DynamicCollider(EditorAction& a)
{
	ComponentHandle<DynamicCollider2DComponent> handle(a.handle);

	if (a.redo)
	{
		handle.GetGameObject().AddComponent<DynamicCollider2DComponent>();

		if (a.current.GetData<bool>())
		{
			handle.GetGameObject().AddComponent<RigidBodyComponent>();
		}
	}
	else
	{
		handle.GetGameObject().DeleteComponent<DynamicCollider2DComponent>();

		if (a.current.GetData<bool>())
		{
			handle.GetGameObject().DeleteComponent<RigidBodyComponent>();
		}
	}
}



// AABB Rectangle check for mouse
//    This checks for non-collision
#define AABB_Mouse_Action(MOUSE_POSITION, BOX_POSITION, BOX_SCALE, ACTION_S)													 \
	if ((MOUSE_POSITION.x > BOX_POSITION.x + (BOX_SCALE.x / 2)) || (MOUSE_POSITION.x < BOX_POSITION.x - (BOX_SCALE.x / 2))  ||   \
		(MOUSE_POSITION.y > BOX_POSITION.y + (BOX_SCALE.y / 2)) || (MOUSE_POSITION.y < BOX_POSITION.y - (BOX_SCALE.y / 2)))      \
	{																															 \
	}																															 \
	else																														 \
	{																															 \
		ACTION_S;																												 \
	}



// Toggle Hitboxes
void debugSetDisplayHitboxes(bool hitboxesShown);

// ErrorList used by the Editor
extern const char *ErrorList[];


Editor::Editor(Engine *engine, GLFWwindow *window) : m_engine(engine), m_objects()
{
	// Log we made it here!
	logger << "Creating Editor.";

	// Toggle off HitBoxes
	debugSetDisplayHitboxes(false);
	debugSetDisplayRaycasts(false);

	// Reserve space for efficiency
	m_objects.reserve(256);

	// Setup the imgui data
	ImGui_ImplGlfwGL3_Init(window, true);

	// Init the style
	ResetStyle();

	// Help Command
	auto help = [this]()
	{
		Editor::Internal_Log("    Commands Avaiable: \n");
		
		for (auto& cmd : m_Console.m_commands)
		{
			Editor::Internal_Log("     - %s \n", cmd.second.command);
		}
	};
	
	// Help screen bs
	RegisterCommand("?", help);
	RegisterCommand("help", help);

	// Create gameobject
	RegisterCommand("create", [this]() 
	{ 
		std::string name = m_Console.m_line.substr(strlen("create "));
		QuickCreateGameObject(name.c_str()); 
	});
	
	// Clear the log
	RegisterCommand("clear", [this]() { m_Console.Clear(); });
	RegisterCommand("cls", [this]() { m_Console.Clear(); });

	// Exit the game, probably should be editor?
	RegisterCommand("exit", [this]() { m_engine->Exit(); });

	RegisterCommand("reload", [this]() {  });

	// Display past commands
	RegisterCommand("history",
	[this]()
	{
		Editor::Internal_Log("    Command History: \n");

		for (auto& history : m_Console.m_log_history)
		{
			Editor::Internal_Log("     - %s \n", history.c_str());
		}
	});

	// Display the current active objects
	RegisterCommand("objects",
	[this]()
	{
		Editor::Internal_Log("    Current Objects: \n");
		for (auto& object : m_objects)
		{
			if (GameObject(object).IsValid())
			{
				Editor::Internal_Log("     - %d : %s \n", object, GameObject(object).GetComponent<ObjectInfo>().Get()->m_name.c_str());
			}
		}
	});

	// Select an object
	RegisterCommand("select",
	[this]()
	{
		int seleted_id = atoi(m_Console.m_line.substr(strlen("select ")).c_str());

		for (auto object : m_objects)
		{
			if (object == seleted_id)
			{
				m_selected_object = object;
				break;
			}
		}
	});

	m_Console.m_log_history.reserve(100);
}


Editor::~Editor()
{
	// Close ImGui
	ImGui_ImplGlfwGL3_Shutdown();

	// Clean up the editor's camera
	delete m_editor_cam;
}


void Editor::Update(float dt)
{
	if (m_editorState.show)
	{
		UpdateGlobalScale();

		debugSetDisplayHitboxes(true);
		debugSetDisplayRaycasts(true);

		if (m_engine->IsWindowTitleDirty())
		{
			m_engine->AppendToWindowTitle(m_filename);
			m_editorState.fileNewFile = false;
		}

		if (m_editorState.first_update)
		{
			prev_camera = Camera::GetActiveCamera();
			
			// Check if the Editor camera needs allocated
			if (m_editor_cam == nullptr)
			{
				// Allocate here so the shaders are init'd and the editor's camera isn't ready too soon
				m_editor_cam = new Camera();
			}

			// Setup the Editor Camera
			m_editor_cam->SetView(glm::vec3(0, 0, 2.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
			m_editor_cam->SetProjection(1.0f, static_cast<float>(Settings::ScreenWidth()) / Settings::ScreenHeight(), 1, 10);
			m_editor_cam->SetPosition(prev_camera->GetPosition());
			m_editorSettings.cameraZoom = prev_camera->GetZoom();
			m_editor_cam->Use();

			m_editorState.first_update = false;
		}
		
		m_editor_cam->SetZoom(0.4f * m_editorSettings.cameraZoom * m_editorSettings.cameraZoom);

		// Check for click events
		OnClick();
		DrawDragBox();
		OnClickRelease();

		// Start a new frame in imgui
		ImGui_ImplGlfwGL3_NewFrame();

		// Get all the active gameobjects
		m_engine->GetSpaceManager()->CollectAllObjects(m_objects);
		SortObjectList();

		// Updates all the popups that could be on screen
		UpdatePopUps(1 / 60.0f);

		// Top Bar
		MenuBar();

		KeyBindings(dt);

		// ImGui::ShowTestWindow();

		// Move, Scale, Rotate
		Tools();

		// Check if we should freeze the world state
		if (m_editorState.freeze)
		{
			m_engine->GetDtObject() = 0;
		}
		else
		{
			//m_engine->GetDtObject() = m_engine->CalculateDt();
		}

		// Render the console
		if (m_editorState.settings)
		{
			SettingsPanel(FAKE_DT);
		}
		
		// Check if we need to print the console
		if (m_editorState.console)
		{
			m_Console.Draw();
		}

		if (m_editorState.ppfx)
		{
			PPFX();
		}

		// Display
		if (m_editorState.objectList)
		{
			ObjectsList();
		}

		// Pass the current object in the editor
		if (!m_multiselect.empty())
		{
			ImGui_GameObject_Multi(m_multiselect, this);
		}
		ImGui_GameObject(m_selected_object, this);

		#ifdef _DEBUG
			// Please don't delete, super important
			if (rand() % 1000000 == 0)
			{
				AddPopUp(PopUpWindow(ErrorList[0], 5.0f, PopUpPosition::Center));
			}
		#endif

		// Render the ImGui frame
		ImGui::Render();

		// Check if we need to auto save yet
		AutoSave(FAKE_DT);

		// Reset the state of things if we are leaving
		if (m_editorState.exiting)
		{
			m_editorState.first_update = true;
			m_editorState.show = false;
			m_editorState.exiting = false;

			debugSetDisplayHitboxes(false);
			debugSetDisplayRaycasts(false);

			prev_camera->Use();
			prev_camera = nullptr;

			if (m_editorState.reload)
			{
				m_editorState.reload = false;
				m_editorState.show = true;
			}
		}
	}
}


// Called whenever the Window is Resized
void Editor::ResizeEvent(int w, int h)
{
	if(m_editor_cam != nullptr)
		m_editor_cam->SetAspectRatio(static_cast<float>(w) / h);
}


void Editor::ResetStyle()
{
	// Style information
	ImGuiStyle * style = &ImGui::GetStyle();

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

	// Window Colors
	style->Colors[ImGuiCol_Text] = ImVec4(0.95f, 0.75f, 0.48f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);

	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.15f, 0.17f, 1.00f);
	style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.20f, 0.29f, 0.37f, 0.94f);

	style->Colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 0.40f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.30f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);

	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.82f, 0.28f, 0.25f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.28f, 0.25f, 1.00f);

	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.32f, 0.22f, 0.22f, 1.00f);

	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.40f, 0.40f, 0.80f, 0.30f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.80f, 0.40f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.80f, 0.50f, 0.50f, 0.40f);

	style->Colors[ImGuiCol_ComboBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.99f);

	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);

	style->Colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);

	style->Colors[ImGuiCol_Button] = ImVec4(0.82f, 0.28f, 0.25f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.82f, 0.28f, 0.25f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.85f, 0.51f, 0.33f, 1.00f);

	style->Colors[ImGuiCol_Header] = ImVec4(0.82f, 0.28f, 0.25f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.82f, 0.28f, 0.25f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.85f, 0.51f, 0.33f, 1.00f);

	style->Colors[ImGuiCol_Separator] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	style->Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
	style->Colors[ImGuiCol_SeparatorActive] = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);

	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);

	style->Colors[ImGuiCol_CloseButton] = ImVec4(0.50f, 0.50f, 0.90f, 0.50f);
	style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.70f, 0.70f, 0.90f, 0.60f);
	style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);

	style->Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);

	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);

	style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

glm::vec2 Editor::GetCamPos()
{
	return m_editor_cam->GetCenter();
}

float Editor::GetUiScale()
{
	return globalScale;
}

void Editor::SetUiScale(float scale)
{
	const float MIN_SCALE = 0.001f;

	if (scale < MIN_SCALE)
	{
		scale = MIN_SCALE;
	}

	float scaleChange = scale / globalScale;

	ImGui::GetIO().FontGlobalScale = scale;

	ImGui::GetStyle().WindowMinSize.x *= scaleChange;
	ImGui::GetStyle().WindowMinSize.y *= scaleChange;

	ImGui::GetStyle().ScrollbarSize *= scaleChange;

	ImGui::GetStyle().GrabMinSize *= scaleChange;

	globalScale = scale;
}

void Editor::UpdateGlobalScale()
{
	if (Input::IsPressed(Key::BracketRight))
	{
		SetUiScale(globalScale * 2);
	}
	else if (Input::IsPressed(Key::BracketLeft))
	{
		SetUiScale(globalScale * 0.5f);
	}
}

void Editor::SortObjectList()
{
	std::sort(m_objects.begin(), m_objects.end(),
		[](GameObject lhs, GameObject rhs)
	{
		float lhs_z = lhs.GetComponent<TransformComponent>()->GetZLayer();
		float rhs_z = rhs.GetComponent<TransformComponent>()->GetZLayer();


		return lhs_z > rhs_z ||
			   ((lhs_z == rhs_z)                 && (lhs.GetName() < rhs.GetName())) ||
			   ((lhs_z == rhs_z) && (lhs.GetName() == rhs.GetName()) && (lhs.Getid()   > rhs.Getid()));
	});
}


// External Log that displays date
void Editor::Log(const char *log_message, ...)
{
	// Buffers to save print data into
	char time_buffer[64];
	char buffer[512];

	// Variadic Stack Data
	va_list args;
	va_start(args, log_message);

	auto t = std::time(nullptr);
	
	// This is the unsafe function warning
	#pragma warning(disable : 4996)
	strftime(time_buffer, 64, "[%H:%M:%S]", std::localtime(&t));

	// Pass the parameters to printf
	vsnprintf(buffer, 512, log_message, args);

	// Append onto the log buffer
	m_Console.m_log_buffer.append("%s - %s\n", time_buffer, buffer);

	va_end(args);

	// Add a line
	m_Console.m_offsets.push_back(m_Console.m_log_buffer.size() - 1);

	// Log the message to the console
	Logging::Log_Editor(Logging::Channel::CORE, Logging::Priority::MEDIUM_PRIORITY, buffer);

	// Tell the console to scroll down
	m_Console.m_scroll = true;
}


// Log Data, works like printf
// This internal log doesnt show datetime
void Editor::Internal_Log(const char * log_message, ...)
{
	// Add to the log buffer
	va_list args;
	va_start(args, log_message);
	
	m_Console.m_log_buffer.appendv(log_message, args);
	
	va_end(args);
	
	m_Console.m_offsets.push_back(m_Console.m_log_buffer.size() - 1);
	m_Console.m_scroll = true;
}


// Check all the Editor KeyBindings
void Editor::KeyBindings(float dt)
{
	// Widget

	if (!ImGui::GetIO().WantCaptureKeyboard)
	{
		if (Input::IsPressed(Key::Q))
		{
			m_gizmo = Editor::Gizmo::none;
		}

		// Translation
		else if (Input::IsPressed(Key::W))
		{
			m_gizmo = Editor::Gizmo::Translation;
		}

		// Scale
		else if (Input::IsPressed(Key::E))
		{
			m_gizmo = Editor::Gizmo::Scale;
		}

		// Rotation
		else if (Input::IsPressed(Key::R))
		{
			m_gizmo = Editor::Gizmo::Rotation;
		}
	}

	// Undo
	if (Input::IsHeldDown(Key::LeftControl) && Input::IsPressed(Key::Z))
	{
		if (m_actions.size)
		{
			Undo_Action();
		}
	}

	// Redo
	if (Input::IsHeldDown(Key::LeftControl) && Input::IsPressed(Key::Y))
	{
		if (m_actions.history.size() > m_actions.size)
		{
			Redo_Action();
		}
	}

	// Save the Level
	if (Input::IsHeldDown(Key::LeftControl) && Input::IsPressed(Key::S))
	{
		if (m_actions.history.size())
		{
			m_editorState.fileSave = true;
		}
	}

	// Save As
	if (Input::IsHeldDown(Key::LeftShift) && Input::IsHeldDown(Key::LeftControl) && Input::IsPressed(Key::S))
	{
		m_editorState.fileSave = true;
		m_editorState.fileNewFile = true;
	}

	// Open Level
	if (Input::IsHeldDown(Key::LeftControl) && Input::IsPressed(Key::O))
	{
		m_editorState.fileLoad = true;
	}


	// Duplicate
	if (Input::IsHeldDown(Key::LeftControl) && Input::IsPressed(Key::D))
	{
		if (m_selected_object)
		{
			m_selected_object = m_selected_object.Duplicate();
		}
	}

	if (Input::IsPressed(Key::Delete))
	{
		if (m_selected_object)
		{
			m_selected_object.Destroy();
		}
	}


	// Move Camera to Object
	if (Input::IsPressed(Key::Space))
	{
		// Move Camera to object
		// m_editor_cam.SetPosition(m_selected_object.GetComponent<TransformComponent>()->GetPosition());
	}

	// Play/Pause the Editor
	if (Input::IsHeldDown(Key::LeftControl) && Input::IsPressed(Key::Space))
	{
		m_editorState.freeze = !m_editorState.freeze;
	}

	// Editor camera controls
	if (Input::IsHeldDown(Key::Right))
	{
		m_editor_cam->SetPosition(m_editor_cam->GetPosition() + glm::vec2(dt * m_editorSettings.cameraArrowSpeed, 0));
	}

	if (Input::IsHeldDown(Key::Left))
	{
		m_editor_cam->SetPosition(m_editor_cam->GetPosition() + glm::vec2(-dt * m_editorSettings.cameraArrowSpeed, 0));
	}

	if (Input::IsHeldDown(Key::Up))
	{
		m_editor_cam->SetPosition(m_editor_cam->GetPosition() + glm::vec2(0, dt * m_editorSettings.cameraArrowSpeed));
	}

	if (Input::IsHeldDown(Key::Down))
	{
		m_editor_cam->SetPosition(m_editor_cam->GetPosition() + glm::vec2(0, -dt * m_editorSettings.cameraArrowSpeed));
	}

	if (Input::IsHeldDown(Key::MouseButton_Right) && !ImGui::GetIO().WantCaptureMouse)
	{
		glm::vec2 mouse = Input::GetMousePos();

		if (!m_editorState.MouseCameraDragClick)
		{
			m_editorState.MouseCameraDragClick = true;
			m_prevMouse = mouse;
		}

		glm::vec2 diff = mouse - m_prevMouse;
		diff.y *= -1; // Screen space is flipped on y.
		// Screen space is much larger than world space.
		diff = (0.0012f * m_editor_cam->GetZoom()) * diff; 

		m_editor_cam->SetPosition(m_editor_cam->GetPosition() - diff /*(diff * dt) * m_editorSettings.cameraSpeed*/);

		m_prevMouse = mouse;
	}

	if (Input::IsReleased(Key::MouseButton_Right))
	{
		m_editorState.MouseCameraDragClick = false;
	}

	if (!ImGui::IsAnyWindowHovered())
	{
		m_editorSettings.cameraZoom -= 0.4f * ImGui::GetIO().MouseWheel;
		m_editorSettings.cameraZoom = max(m_editorSettings.cameraZoom, 0.2);
	}
}


// Adds an action to the history
//   Order is Old Value, New Value, Field Name, handle to component, action function
void Editor::Push_Action(EditorAction&& action)
{
	// Size keeps track of the number of actions
	if (m_actions.size)
	{
		// Check if any undo has been done
		if (m_actions.size == m_actions.history.size())
		{
			m_actions.history.emplace_back(action);
			++m_actions.size;
		}
		else
		{

			// Delete all the old actions
			while (m_actions.size != m_actions.history.size())
			{
				m_actions.history.pop_back();
			}

			// We need to catch back up to the actual size
			m_actions.history.emplace_back(action);
			++m_actions.size;
		}
	}
	else
	{
		// Zero actions saved, so just start at the beginning
		m_actions.history.clear();
		m_actions.history.emplace_back(action);
		++m_actions.size;
	}

	m_editorState.fileDirty = true;
}


void Editor::Undo_Action()
{
	// Go to previous action
	m_actions.history[m_actions.size - 1].redo = false;

	// Call the resolve function
	m_actions.history[m_actions.size - 1].func(m_actions.history[m_actions.size - 1]);
	--m_actions.size;

	if (m_actions.size == 0)
	{
		m_editorState.fileDirty = false;
	}
}


void Editor::Redo_Action()
{
	// Set the redo flag
	m_actions.history[m_actions.size].redo = true;

	// Call the resolve function
	m_actions.history[m_actions.size].func(m_actions.history[m_actions.size]);

	// We redid an action, so it is valid to be undone
	++m_actions.size;

	if (m_actions.size != m_actions.history.size())
	{
		m_editorState.fileDirty = true;
	}
}


void Editor::QuickCreateGameObject(const char *name, glm::vec2& pos, glm::vec2& size)
{
	if (m_selected_object)
	{
		GameObject object = m_engine->GetSpace(m_selected_object.GetIndex())->NewGameObject(name);

		// Add a transform component
		object.AddComponent<TransformComponent>(glm::vec3(pos.x, pos.y, 1.0f), glm::vec3(size.x, size.y, 1.0f));

		m_selected_object = object.Getid();
	}
}

// Overload creates at camera position.
void Editor::QuickCreateGameObject(const char *name)
{
	QuickCreateGameObject(name, m_editor_cam->GetCenter());
}


void Editor::SetGameObject(GameObject new_object)
{
	m_selected_object = new_object.Getid();
}

void Editor::TrySelect(const glm::vec2& mouse)
{
	// Position and Scale of the Current Object
	glm::vec2 pos;
	glm::vec2 scale;

	bool multiselecting = Input::IsHeldDown(Key::LeftShift) || Input::IsHeldDown(Key::RightShift);

	// If we're not holding shift stop multiselecting
	if (!multiselecting)
	{
		m_multiselect.clear();
	}

	// Check EVERY object
	for (auto id : m_objects)
	{
		// Get the GameObject id
		GameObject object = id;

		if (object.IsValid())
		{
			// Transform handle
			ComponentHandle<TransformComponent> transform = object.GetComponent<TransformComponent>();
			scale = abs(transform->GetScale());
			pos = transform->GetPosition();

			// Check for non-collision
			if (mouse.x > pos.x + (scale.x / 2) || mouse.x < pos.x - (scale.x / 2) ||
				mouse.y > pos.y + (scale.y / 2) || mouse.y < pos.y - (scale.y / 2))
			{
				continue;
			}

			GameObject obj = transform.GetGameObject();
			// If we already have the object while holding shift we're trying to deselect.
			if (multiselecting)
			{
				// First object should be deselected.
				if (obj == m_selected_object)
				{
					m_selected_object = m_multiselect.back().first;
					m_multiselect.pop_back();
					transform = m_selected_object.GetComponent<TransformComponent>();

					// Update the offsets.
					for (auto& multiPair : m_multiselect)
					{
						multiPair.second = multiPair.first.GetComponent<TransformComponent>()->GetPosition() - transform->GetPosition();
					}
					return;
				}
				// Check each other object to be deselected.
				for (auto iter = m_multiselect.begin(); iter != m_multiselect.end(); ++iter)
				{
					if (iter->first == obj)
					{
						m_multiselect.erase(iter);
						return;
					}
				}
			}

			// Save the GameObject data
			if (!multiselecting || m_selected_object == GameObject(0))
			{
				m_selected_object = obj;
			}
			else
			{
				m_multiselect.push_back(std::make_pair(transform.GetGameObject(),transform->GetPosition() - m_selected_object.GetComponent<TransformComponent>()->GetPosition()));
			}

			return;
		}
	}

	// Deselect the object if nothing was found
	if (!multiselecting)
	{
		m_selected_object = 0;
	}

	// Start dragging
	m_select_dragging = true;
	m_drag_select_start = mouse;
}

void Editor::OnClick()
{// Check for mouse 1 click
	if (Input::IsPressed(Key::Mouse_1))
	{
		// Get the Mouse Position, save it as a previous position as well
		const glm::vec2 mouse = Input::GetMousePos_World();
		m_prevMouse = mouse;

		// Check if ImGui needs the mouse
		if (ImGui::IsAnyWindowHovered())
		{
			m_editorState.imguiWantMouse = true;
			return;
		}
		else
		{
			// ImGui does not want the mouse
			m_editorState.imguiWantMouse = false;
		}

		// Only try to select something if we're not worried about the tool or we're not transforming.
		if (m_editorSettings.selectWithTransformTools || m_gizmo == Gizmo::none)
		{
			TrySelect(mouse);
		}
	}
}

void Editor::DrawDragBox()
{
	if (m_select_dragging)
	{
		const glm::vec2 mouse = Input::GetMousePos_World();
		glm::vec2 scale = mouse - m_drag_select_start;
		scale.x = abs(scale.x);
		scale.y = abs(scale.y);

		DebugGraphic::DrawSquare((mouse + m_drag_select_start) * (1.0f / 2), scale);
	}
}

void Editor::OnClickRelease()
{
	if (Input::IsReleased(Key::Mouse_1))
	{
		if (m_select_dragging)
		{
			m_select_dragging = false;

			// Clear the current multiselect if we're not trying to multiselect more.
			if (!Input::IsHeldDown(Key::RightShift) && !Input::IsHeldDown(Key::LeftShift))
			{
				m_multiselect.clear();
			}

			// Get drag bounds.
			const glm::vec2 selectEnd = Input::GetMousePos_World();

			float left = m_drag_select_start.x;
			float right = selectEnd.x;
			float top = m_drag_select_start.y;
			float bot = selectEnd.y;

			if (left > right)
			{
				std::swap(left, right);
			}
			if (bot > top)
			{
				std::swap(bot, top);
			}

			// Check EVERY object
			for (auto id : m_objects)
			{
				// Get the GameObject id
				GameObject object = id;

				if (object.IsValid())
				{
					// Transform handle
					ComponentHandle<TransformComponent> transform = object.GetComponent<TransformComponent>();
					glm::vec2 scale = abs(transform->GetScale());
					glm::vec2 pos = transform->GetPosition();

					// Check for non-collision
					if (pos.x + (scale.x / 2) < left
					 || pos.x - (scale.x / 2) > right
					 || pos.y + (scale.y / 2) < bot
					 || pos.y - (scale.y / 2) > top)
					{
						continue;
					}

					// Save the GameObject data
					if (m_selected_object == INVALID_GAMEOBJECT_ID)
					{
						m_selected_object = transform.GetGameObject();
					}
					else
					{
						m_multiselect.push_back(std::make_pair(transform.GetGameObject(), transform->GetPosition() - m_selected_object.GetComponent<TransformComponent>()->GetPosition()));
					}
				}
			}
		}
	}
}

// Averages between positions
static glm::vec2 Average_Pos(Array<GameObject_ID, MAX_SELECT>& objects)
{
	glm::vec2 avg;

	for (size_t i = 0; i < objects.m_size; i++)
	{
		// f = t * b + a(1 - t)
		avg += GameObject(objects[i]).GetComponent<TransformComponent>()->GetPosition();
	}
	
	return avg / glm::vec2(static_cast<float>(objects.m_size));
}


void Editor::Tools()
{
	// Static variables to keep track of editing state.
	static bool editingObject = false;
	static float initialObjRotation;
	static glm::vec2 initialMousePos;
	static glm::vec2 initialObjPos;

	if (m_selected_object)
	{
		ComponentHandle<TransformComponent> transform = m_selected_object.GetComponent<TransformComponent>();

		// Activate snap if the player is holding control.
		bool snap = m_editorSettings.snap;
		if (Input::IsHeldDown(Key::LeftControl) || Input::IsHeldDown(Key::RightControl))
		{
			snap = !snap;
		}

		// Get the position of the object.
		glm::vec2 pos = transform->GetPosition();

		switch (m_gizmo)
		{

		case Gizmo::Translation:
		{
			// delta Mouse Pos
			glm::vec2 mouse = Input::GetMousePos_World();

			const glm::vec2 pos_x_dir = glm::vec2(pos.x + 0.125f, pos.y);
			const glm::vec2 scale_x_dir(0.25f, 0.1f);

			const glm::vec2 pos_y_dir = glm::vec2(pos.x, pos.y + +0.125f);
			const glm::vec2 scale_y_dir(0.1f, 0.25f);

			DebugGraphic::DrawSquare(pos_x_dir, scale_x_dir);

			DebugGraphic::DrawSquare(pos_y_dir, scale_y_dir);

			if (Input::IsHeldDown(Key::Mouse_1) && !m_editorState.imguiWantMouse)
			{
				GameObject object = m_selected_object;
				glm::vec2 targetPos = transform->GetPosition();

				// If we're just clicking the object set up the initial state.
				if (!editingObject)
				{
					editingObject = true;
					initialObjPos = targetPos;
					initialMousePos = mouse;
				}

				// delta Mouse Pos
				glm::vec2 mouseDiff = mouse - pos;
				glm::vec2 initialMouseDiff = initialMousePos - initialObjPos;

				// Check if we need to save the old value
				if (!m_editorState.MouseDragClick)
				{
					bool freeze_axis = false;

					// AABB of the box to only allow movement on the x-axis
					if (mouse.x < pos_x_dir.x + (scale_x_dir.x / 2) && mouse.x > pos_x_dir.x - (scale_x_dir.x / 2))
					{
						if (mouse.y < pos_x_dir.y + (scale_x_dir.y / 2) && mouse.y > pos_x_dir.y - (scale_x_dir.y / 2))
						{
							m_transformDir = EditorGizmoDirection::Dir_X;
							freeze_axis = true;
							DebugGraphic::DrawSquare(pos_x_dir, scale_x_dir, 0, glm::vec4(HexVec(0x0000FF), 1));
						}
					}

					// AABB of the box to only allow movement on the y-axis
					if (mouse.x < pos_y_dir.x + (scale_y_dir.x / 2) && mouse.x > pos_y_dir.x - (scale_y_dir.x / 2))
					{
						if (mouse.y < pos_y_dir.y + (scale_y_dir.y / 2) && mouse.y > pos_y_dir.y - (scale_y_dir.y / 2))
						{
							m_transformDir = EditorGizmoDirection::Dir_Y;
							freeze_axis = true;
							DebugGraphic::DrawSquare(pos_y_dir, scale_y_dir, 0, glm::vec4(HexVec(0x0000FF), 1));
						}
					}

					// Default to allow both, and auto-unfreeze
					if (!freeze_axis)
					{
						m_transformDir = EditorGizmoDirection::Both;
					}

					objectSave.SetLocalPosition(object.GetComponent<TransformComponent>()->GetRelativePosition());
					m_editorState.MouseDragClick = true;
				}


				// Keeping track of direction allows for us to freeze the other axes
				switch (m_transformDir)
				{
					// Freeze the y-axis and allow movement in the x-axis
				case EditorGizmoDirection::Dir_X:
					targetPos = glm::vec2(pos.x + mouseDiff.x - initialMouseDiff.x, pos.y);
					DebugGraphic::DrawSquare(pos_x_dir, scale_x_dir, 0, glm::vec4(HexVec(0xFFFF00), 1));

					break;

					// Freeze the x-axis and allow movement in the y-axis
				case EditorGizmoDirection::Dir_Y:
					targetPos = glm::vec2(pos.x, pos.y + mouseDiff.y - initialMouseDiff.y);

					DebugGraphic::DrawSquare(pos_y_dir, scale_y_dir, 0, glm::vec4(HexVec(0xFFFF00), 1));
					break;

					// Move by both
				case EditorGizmoDirection::Both:

					// Add the mouseDiff to the position
					targetPos = pos + mouseDiff - initialMouseDiff;

				default:
					break;
				}

				m_prevMouse = Input::GetMousePos_World();

				// Calculate the nearest snap point.
				if (snap)
				{
					targetPos.x -= fmod(targetPos.x, m_editorSettings.snapInterval);
					targetPos.y -= fmod(targetPos.y, m_editorSettings.snapInterval);

					if (!m_editorSettings.absoluteSnap)
					{
						targetPos += glm::vec2(fmod(initialObjPos.x, m_editorSettings.snapInterval), fmod(initialObjPos.y, m_editorSettings.snapInterval));
					}
				}

				transform->SetPosition(targetPos);
				for (const auto& objPair : m_multiselect)
				{
					objPair.first.GetComponent<TransformComponent>()->SetPosition(targetPos + objPair.second);
				}

				return;
			}
			else
			{
				editingObject = false;
			}

			if (Input::IsReleased(Key::Mouse_1) && m_editorState.MouseDragClick)
			{
				ComponentHandle<TransformComponent> handle = m_selected_object.GetComponent<TransformComponent>();
				EditorAction undoAction = { glm::vec2(objectSave.GetRelativePosition()), glm::vec2(handle->GetRelativePosition()), "position",{ m_selected_object, true }, Action_General<TransformComponent, glm::vec2> };

				if (m_multiselect.empty())
				{
					Push_Action(std::move(undoAction));
				}
				else
				{
					Push_Action(MultiselectTransformUndo::CreateAction(undoAction, m_multiselect));
				}
				
				// Reset the click state
				m_editorState.MouseDragClick = false;
			}

			break;
		}
		case Gizmo::Scale:
		{
			GameObject object = m_selected_object;

			// Mouse Position in World Coordinates
			glm::vec2 mouse = Input::GetMousePos_World();

			// Scale of the Object
			glm::vec2 scale = object.GetComponent<TransformComponent>()->GetScale();

			// Scales of the boxes drawn
			const glm::vec2 box_scale(0.15f, 0.15f);
			const glm::vec2 box_scale_smaller(0.14f, 0.14f);

			// Position of the boxes
			glm::vec2 box_pos(pos.x + (scale.x / 2) - (0.15f / 2), pos.y);

			// Detect which direction

			// DIRECTION
			//    Draw a Square to Represent the space to grab
			//    Draw Other square to improve visability
			//    AABB Mouse Detection

			// X Direction
			DebugGraphic::DrawSquare(box_pos, box_scale, 0.0f, glm::vec4(HexVec(0xFFFF00), 1));
			DebugGraphic::DrawSquare(box_pos, box_scale_smaller, 0.0f, glm::vec4(HexVec(0xFFFF00), 1));
			AABB_Mouse_Action(mouse, box_pos, box_scale, m_scaleDir = EditorGizmoDirection::Dir_X);
			

			box_pos = glm::vec2(pos.x, pos.y + (scale.y / 2) - (0.15f / 2));
			// Y Direction
			DebugGraphic::DrawSquare(box_pos, box_scale, 0.0f, glm::vec4(HexVec(0xFF0000), 1));
			DebugGraphic::DrawSquare(box_pos, box_scale_smaller, 0.0f, glm::vec4(HexVec(0xFF0000), 1));
			AABB_Mouse_Action(mouse, box_pos, box_scale, m_scaleDir = EditorGizmoDirection::Dir_Y);


			box_pos.x += (scale.x / 2) - (0.15f / 2);
			// Both
			DebugGraphic::DrawSquare(box_pos, box_scale, 0.0f, glm::vec4(HexVec(0x000000), 1));
			DebugGraphic::DrawSquare(box_pos, box_scale_smaller, 0.0f, glm::vec4(HexVec(0x000000), 1));
			AABB_Mouse_Action(mouse, box_pos, box_scale, m_scaleDir = EditorGizmoDirection::Both);



			if (Input::IsHeldDown(Key::Mouse_1) && !m_editorState.imguiWantMouse)
			{
				// delta Mouse pos
				glm::vec2 mouseChange = mouse - m_prevMouse;

				// Check if we need to save the old value of the object
				if (!m_editorState.MouseDragClick && m_prevMouse != mouse)
				{
					objectSave.SetScale(object.GetComponent<TransformComponent>()->GetScale());
					m_editorState.MouseDragClick = true;
				}

				// Scale Gizmo Actions

				// Figure out which way to scale
				switch (m_scaleDir)
				{
				case EditorGizmoDirection::Dir_X:
					//if (Input::IsHeldDown(Key::LeftControl))
					//{
						scale.x += mouseChange.x;
					//}
					//else
					//{
					//	transform->SetPosition(transform->GetPosition() + glm::vec2(mouseChange.x / 2.0f, 0));
					//	scale.x += mouseChange.x;
					//}
					
					break;

				case EditorGizmoDirection::Dir_Y:
					scale.y += mouseChange.y;
					break;

				case EditorGizmoDirection::Both:
					if (Input::IsHeldDown(Key::LeftShift))
					{
						float ratio = scale.y / scale.x;
						scale.x += mouseChange.x;
						scale.y += ratio * mouseChange.x;
					}
					else
					{
						scale.x += mouseChange.x;
						scale.y += mouseChange.y;
					}
					break;

				case EditorGizmoDirection::Invalid:

					break;

				default:
					logger << "[EDITOR] Invalid Axis sent to Scale Gizmo.\n";
					break;
				}

				object.GetComponent<TransformComponent>()->SetScale(glm::vec3(scale, 1));

				m_prevMouse = mouse;
			}

			if (Input::IsReleased(Key::Mouse_1) && m_editorState.MouseDragClick)
			{
				// Get the transform handle
				ComponentHandle<TransformComponent> handle = m_selected_object.GetComponent<TransformComponent>();

				// Push the action using the Scale Before and the scale after
				Push_Action({ objectSave.GetScale(), handle->GetScale(), "scale", { m_selected_object, true }, Action_General<TransformComponent, glm::vec3> });

				// Reset the Mouse Click
				m_editorState.MouseDragClick = false;

				// Reset the scale direction
				m_scaleDir = Invalid;
			}

			break;
		}

		case Gizmo::Rotation:
		{
			GameObject object = m_selected_object;
			ComponentHandle<TransformComponent> transform = object.GetComponent<TransformComponent>();

			// Decided to just default to the largest scale value
			float circleRadius = max(object.GetComponent<TransformComponent>()->GetScale().x, object.GetComponent<TransformComponent>()->GetScale().y);
			circleRadius /= 3.0f;

			// Gizmo circle
			DebugGraphic::DrawCircle(pos, circleRadius, glm::vec4(HexVec(0xFFFF00), 1));
			DebugGraphic::DrawCircle(pos, circleRadius - 0.01f, glm::vec4(HexVec(0xFFFF00), 1));

			if (Input::IsHeldDown(Key::Mouse_1) && !m_editorState.imguiWantMouse)
			{
				// Mouse Position
				glm::vec2 mouse = Input::GetMousePos_World();

				//Initialize if we just started rotating
				if (!editingObject)
				{
					editingObject = true;
					initialObjRotation = transform->GetRotation();
					initialMousePos = mouse;
				}

				// Distance between mouse and object
				glm::vec2 mouseOffset = mouse - pos;
				float squareOffsetLength = glm::dot(mouseOffset, mouseOffset);

				// Rotation Gizmo Actions
				if (squareOffsetLength < circleRadius * circleRadius /*&& dx + dy > (circle * circle - 0.5f)*/)
				{
					// Check if we need to save the current value
					if (!m_editorState.MouseDragClick && m_prevMouse != mouse)
					{
						objectSave.SetRotation(object.GetComponent<TransformComponent>()->GetRotation());
						m_editorState.MouseDragClick = true;
					}
				}

				// Get the change in rotation of the mouse since we first started rotating.
				float mouseRotation = atan2(mouseOffset.y, mouseOffset.x);
				glm::vec2 initialMouseOffset = initialMousePos - pos;
				float initialMouseRotation = atan2(initialMouseOffset.y, initialMouseOffset.x);
				float rotationDiff = mouseRotation - initialMouseRotation;
				rotationDiff *= (180.0f / PI);

				// Figure out the target rotation and convert to degrees.
				float finalRotation = fmod(initialObjRotation + rotationDiff + 360, 360);

				if (snap)
				{
					float rotSnap = m_editorSettings.rotationSnapInterval;
					finalRotation = finalRotation + rotSnap / 2 - fmod(finalRotation + rotSnap / 2, rotSnap);

					if (!m_editorSettings.absoluteSnap)
					{
						finalRotation += fmod(initialObjRotation, rotSnap);
					}
				}

				object.GetComponent<TransformComponent>()->SetRotation(finalRotation);
				m_prevMouse = mouse;
			}
			else
			{
				editingObject = false;
			}

			// Check if the user is done with the click and push the action
			if (Input::IsReleased(Key::Mouse_1) && m_editorState.MouseDragClick)
			{
				ComponentHandle<TransformComponent> handle = m_selected_object.GetComponent<TransformComponent>();
				Push_Action({ objectSave.GetRotation(), handle->GetRotation(), "rotation", { m_selected_object, true }, Action_General<TransformComponent, float> });
				m_editorState.MouseDragClick = false;
			}

			break;
		}

		case Gizmo::none:
			break;

		default:
			logger << "[EDITOR] Invalid tool value.\n";
			break;
		}
	}
}


void Editor::AddPopUp(PopUpWindow&& pop)
{
	IncrementPopUpCount(pop.pos);
	m_pop_ups.emplace_back(pop);
}


void Editor::UpdatePopUps(float dt)
{
	// Here for Popup Testing
	// if (Input::IsPressed(Key::I))
	// {
	//		AddPopUp(PopUpWindow("Test ", 2.0f, PopUpPosition::Mouse));
	// }

	int width = 0;
	int height = 0;
	glfwGetWindowSize(m_engine->GetWindow(), &width, &height);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);

	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_ShowBorders |
		ImGuiWindowFlags_NoScrollbar;


	for (int i = 0; i < m_pop_ups.size(); ++i)
	{
		PopUpWindow& popup = m_pop_ups[i];

		// float offset = m_pop_ups.size() - i;
		glm::vec2 padding(65, 65);
		
		ImVec2 pos;
		ImVec2 size(ImGui::CalcTextSize(popup.message.c_str()).x * 1.49f, static_cast<float>(42 * GetPopUpCount(popup.pos)));
		float text_padding = size.x;  //strlen(popup.message) * 5.75f;

		// Find where to draw the popup
		switch (popup.pos)
		{
		case PopUpPosition::BottomLeft:
			pos = ImVec2(padding.x, static_cast<float>(height) - padding.y);
			break;

		case PopUpPosition::BottomRight:
			pos = ImVec2(static_cast<float>(width) - padding.x - text_padding, static_cast<float>(height) - padding.y);
			break;

		case PopUpPosition::TopRight:
			pos = ImVec2(static_cast<float>(width) - padding.x - text_padding, padding.y);
			break;

		case PopUpPosition::TopLeft:
			pos = ImVec2(padding.x, padding.y);
			break;

		case PopUpPosition::Center:
			pos = ImVec2(width / 2.0f, height / 2.0f);
			break;
		case PopUpPosition::Mouse:
			pos = Input::GetMousePos();
			break;
		}

		// Check if the popup needs to be removed
		if (popup.timer > 0)
		{
			// f = t * b + a(1 - t)
			constexpr float factor = 0.25f;

			// Do fading when we are close to the end, but none until then
			float alpha = 1.0f;
			if (popup.timer < 0.25f && GetPopUpCount(popup.pos) == 1)
			{
				alpha = (popup.timer / popup.max_time) * factor + popup.alpha * (1 - factor);
			}

			popup.timer -= dt;

			// Setup next window, including a special alpha value
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
			ImGui::SetNextWindowPos(pos);
			ImGui::SetNextWindowSize(ImVec2(globalScale * size.x, globalScale * size.y));

			ImGui::Begin(popup.message.c_str(), nullptr, flags);
			ImGui::PushAllowKeyboardFocus(false);

			ImGui::Text(popup.message.c_str());

			// Save the alpha for next frame
			popup.alpha = alpha;

			// Clean Up
			ImGui::PopAllowKeyboardFocus();
			ImGui::End();
			ImGui::PopStyleVar(1);
		}
		else
		{
			DecrementPopUpCount(popup.pos);
			m_pop_ups.erase(m_pop_ups.begin() + i);
		}
	}

	// Clean Up
	ImGui::PopStyleVar(1);
}


int Editor::GetPopUpCount(PopUpPosition pos)
{
	return m_PopUpCount[pos];
}


void Editor::IncrementPopUpCount(PopUpPosition pos)
{
	++m_PopUpCount[pos];
}


void Editor::DecrementPopUpCount(PopUpPosition pos)
{
	Assert(m_PopUpCount[pos] >= 0);

	--m_PopUpCount[pos];
}


void Editor::PrintObjects()
{
	// Get all the names of the objects
	char name_buffer[128] = { 0 };
	GameObject object(0);

	for (auto& object_id : m_objects)
	{
		// Use invalid gameObject as a delimiter
		//if (object_id == INVALID_GAMEOBJECT_ID)
		//{
		//	ImGui::Separator();
		//	continue;
		//}
		
		// Assign the GameObject to a temp
		object = object_id;

		// Check if the object exist (checks if destoryed)
		if (object.IsValid())
		{
			// Grab the name to display
			std::string& name = object.GetComponent<ObjectInfo>().Get()->m_name;

			// Save the buffer based off name size, max name size is 8
			if (name.size() > 10)
			{
				snprintf(name_buffer, sizeof(name_buffer),
					"%-10.10s... - %d : %d : %d", name.c_str(), object.GetIndex(), static_cast<int>(object.GetComponent<TransformComponent>()->GetZLayer()), object.GetObject_id());
			}
			else
			{
				snprintf(name_buffer, sizeof(name_buffer),
					"%-13.13s - %d : %d : %d", name.c_str(), object.GetIndex(), static_cast<int>(object.GetComponent<TransformComponent>()->GetZLayer()), object.GetObject_id());
			}

			if (!object.IsActive())
			{
				ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(1,1,1,0.6f));
			}

			if (GetSearchBars().objects.PassFilter(name_buffer))
			{
				if (ImGui::Selectable(name_buffer))
				{
					m_multiselect.clear();
					SetGameObject(object);

					if (!object.IsActive())
					{
						ImGui::PopStyleColor();
					}
				}
			}

			if (!object.IsActive())
			{
				ImGui::PopStyleColor();
			}
		}
	}
}


void Editor::ObjectsList()
{
	using namespace ImGui;

	SetNextWindowSize(ImVec2(globalScale * 260, globalScale * 400));
	SetNextWindowPos(ImVec2(0, 30), ImGuiCond_Once);
	Begin("Objects", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize);

	if (Button("Create"))
	{
		ImGui::OpenPopup("Create GameObject###CreateGameObject");
	} 

	if (ImGui::BeginPopup("Create GameObject###CreateGameObject"))
	{
		static char name[128] = { 'N', 'o', ' ', 'N', 'a', 'm', 'e', '\0' };
		if (ImGui::InputText("Name", name, sizeof(name), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			GameObject object = m_engine->GetSpace(m_current_space_index)->NewGameObject(name);

			// Add a transform component
			object.AddComponent<TransformComponent>(glm::vec3(m_editor_cam->GetPosition(), 0));

			m_selected_object = object.Getid();

			ImGui::CloseCurrentPopup();
		}

		ImGui::SliderInt("Space", &m_current_space_index, 0, static_cast<int>(m_engine->GetSpaceManager()->GetSize()) - 1);

		if (ImGui::Button("Create###createObjectListButton"))
		{
			GameObject object = m_engine->GetSpace(m_current_space_index)->NewGameObject(name);

			// Add a transform component
			object.AddComponent<TransformComponent>(glm::vec3(m_editor_cam->GetPosition(), 0));

			m_selected_object = object.Getid();

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
	
	SameLine();

	if (Button("Add Space"))
	{
		engine->GetSpaceManager()->AddSpace();
	}

	GetSearchBars().objects.Draw("Search##objects", 150);

	ImGui::BeginChild("object_list_print");
	PrintObjects();
	ImGui::EndChild();
	End();
}


void Editor::Console::SetActive(ImGuiTextEditCallbackData *data, size_t entryIndex)
{
	// Copy in the data  from the command
	memmove(data->Buf, m_commands[entryIndex].command, m_commands[entryIndex].cmd_length);

	// Update the Buffer data
	data->Buf[m_commands[entryIndex].cmd_length] = '\0';
	data->BufTextLen = static_cast<int>(m_commands[entryIndex].cmd_length);
	data->BufDirty = true;
}


void Editor::Console::SetActive_History(ImGuiTextEditCallbackData *data, int entryIndex)
{
	// Copy in the data  from the command
	memmove(data->Buf, m_log_history[entryIndex].c_str(), m_log_history[entryIndex].size());

	// Update the Buffer data
	data->Buf[m_log_history[entryIndex].size()] = '\0';
	data->BufTextLen = static_cast<int>(m_log_history[entryIndex].size());
	data->BufDirty = true;
}


void Editor::Console::SetActive_Completion(ImGuiTextEditCallbackData *data, int entryIndex)
{
	// Copy in the data  from the command
	memmove(data->Buf, m_matches[entryIndex], strlen(m_matches[entryIndex]));

	// Update the Buffer data
	data->Buf[strlen(m_matches[entryIndex])] = '\0';
	data->BufTextLen = static_cast<int>(strlen(m_matches[entryIndex]));
	data->BufDirty = true;
}


void Editor::Console::SetInput_Blank(ImGuiTextEditCallbackData *data)
{
	// Copy in the data  from the command
	// memmove(data->Buf, "", 0);

	// Update the Buffer data
	data->Buf[0] = '\0';
	data->BufTextLen = 0;
	data->BufDirty = true;
}


// Alternative StrCmp
bool Editor::Console::Command_StrCmp(const char *str1, const char *str2)
{
	// Check if the current letters are the same
	while (!(*str1 ^ *str2++))

		// If end of str1 is reached, return true
		if (!*str1++)
			return true;

	return false;
}


// This is from imgui, it returns how much alike two strings are
//    This function works magic
int Editor::Console::Strnicmp(const char* str1, const char* str2, int n)
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


// External Log that displays date
void Editor::Console::Log(const char *log_message, ...)
{
	// Buffers to save print data into
	char time_buffer[64];
	char buffer[512];

	// Variadic Stack Data
	va_list args;
	va_start(args, log_message);

	auto t = std::time(nullptr);

	// This is the unsafe function warning
#pragma warning(disable : 4996)
	strftime(time_buffer, 64, "[%H:%M:%S]", std::localtime(&t));

	// Pass the parameters to printf
	vsnprintf(buffer, 512, log_message, args);

	// Append onto the log buffer
	m_log_buffer.append("%s - %s\n", time_buffer, buffer);

	va_end(args);

	// Add a line
	m_offsets.push_back(m_log_buffer.size() - 1);

	// Log the message to the console
	Logging::Log_Editor(Logging::Channel::CORE, Logging::Priority::MEDIUM_PRIORITY, buffer);

	// Tell the console to scroll down
	m_scroll = true;
}


// Log Data, works like printf
// This internal log doesnt show datetime
void Editor::Console::Internal_Log(const char * log_message, ...)
{
	// Add to the log buffer
	va_list args;
	va_start(args, log_message);

	// Append the log message to the buffer
	m_log_buffer.appendv(log_message, args);

	va_end(args);

	m_offsets.push_back(m_log_buffer.size() - 1);
	m_scroll = true;
}


// Callback whenever something is typed into the console
//    Mostly used for the popup being displayed
int Editor::Console::Input_Callback(ImGuiTextEditCallbackData *data)
{
	// Get the console pointer we passed in
	Console *console = reinterpret_cast<Console *>(data->UserData);

	// Check which event happened
	switch (data->EventFlag)
	{
		// When you hit tab
	case ImGuiInputTextFlags_CallbackCompletion:
		if (console->m_matches.size())
		{
			// Delete the input buffer
			data->DeleteChars(0, static_cast<int>(strlen(data->Buf)));

			// Insert the command into the input buffer
			data->InsertChars(data->CursorPos, console->m_matches[0], console->m_matches[0] + strlen(console->m_matches[0]));
		}
		break;

		// History based data, when you hit up or down arrow keys
	case ImGuiInputTextFlags_CallbackHistory:
			// editor->m_state.m_popUp = true;
			
			// Check if arrow keys are pressed and if the current item is at the end
			if (data->EventKey == ImGuiKey_UpArrow && (console->m_state.activeIndex < static_cast<int>(console->m_log_history.size() - 1)))
			{
				// Increase the active index and copy in the command
				console->m_state.activeIndex++;
				console->SetActive_History(data, static_cast<int>(console->m_log_history.size() - console->m_state.activeIndex) - 1);
			}
			else if (data->EventKey == ImGuiKey_DownArrow && (console->m_state.activeIndex > -1))
			{
				// Decrease the state and copy in "" if nothing is left, otherwise copy in the command
				console->m_state.activeIndex--;

				// Check if we are in an inactive state
				if (console->m_state.activeIndex == -1)
				{
					// Erase everything in the input
					SetInput_Blank(data);
				}
				else
				{
					// Set the next item in history
					console->SetActive_History(data, static_cast<int>(console->m_log_history.size() - console->m_state.activeIndex) - 1);
				}
			}
		break;

		// This happens all the time
	case ImGuiInputTextFlags_CallbackAlways:
			// Clear the data in the matches vector, but dont free the alloc'd memory
			console->m_matches.clear();

			// Make sure there is data to read from
			if (data->Buf && data->BufTextLen > 0)
			{
				// If there is data then the popup needs to be drawn
				console->m_state.m_popUp = true;

				// Get the word from the data
				const char *word_end = data->Buf + data->CursorPos;
				const char *word_start = word_end;

				// Find the beginning and end of the word
				while (word_start > data->Buf)
				{
					// Start at the last letter
					const char c = word_start[-1];

					// End of word
					if (c == '\t' || c == ',' || c == ';')
						break;

					// Move back a letter
					--word_start;
				}

				// Check the word against commands for matches
				for (auto& command : console->m_commands)
				{
					if (Strnicmp(command.second.command, word_start, static_cast<int>((word_end - word_start))) == 0)
					{
						// Save the matches
						console->m_matches.push_back(command.second.command);
					}
				}


				// Check if there were matches
				if (console->m_matches.Data)
				{
					// Get the length of the match
					int length = static_cast<int>(word_end - word_start);

					// Check for length on matches
					while (length)
					{
						char character = 0;
						bool all_matches_made = true;

						// Print out the matches
						for (int i = 0; i < console->m_matches.Size && all_matches_made; ++i)
						{
							// Read a character
							if (i == 0 && length < console->m_matches.Size)
							{
								character = console->m_matches[i][length];
							}

							// Check if are finished with comparing the strings
							else if (character == 0 || character != console->m_matches[i][length])
							{
								all_matches_made = false;
							}
						}

						// Break when we are done
						if (!all_matches_made)
						{
							break;
						}
						length++;
					}

					// Input the selected command into the input buffer
					if (length > 0)
					{
						data->DeleteChars(static_cast<int>(word_start - data->Buf), static_cast<int>(word_end - word_start));
						data->InsertChars(data->CursorPos, console->m_matches[0], console->m_matches[0] + length);
					}
				}
			}

			// Get the clicked indexed item and reset the popup state
			if (console->m_state.clickedIndex != -1)
			{
				console->SetActive(data, console->m_state.clickedIndex);
				console->m_state.activeIndex = -1;
				console->m_state.clickedIndex = -1;
				console->m_state.m_popUp = false;
			}
		break;

		// When you type a new character; here for possible future use
	//case ImGuiInputTextFlags_CallbackCharFilter:
		//break;

	default:
		break;
	}

	return 0;
}


// Toggle the flag that Shows/Hides the Editor
void Editor::ToggleEditor()
{
	if (!m_editorState.show)
	{
		m_editorState.show = true;
	}
	else
	{
		m_editorState.exiting = true;
	}


}


// Menubar running the top of the screen.
//    Serves as a home for certain actions
void Editor::MenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		// File -- Standard on all MenuBars
		if (ImGui::BeginMenu("File"))
		{
			// Save the current game
			if (ImGui::MenuItem("Save"))
			{
				m_editorState.fileSave = true;
			}

			if (ImGui::MenuItem("Save As..."))
			{
				m_editorState.fileOpened = false;
				m_editorState.fileSave = true;
			}

			// Load an file
			if (ImGui::MenuItem("Load"))
			{
				m_editorState.fileLoad = true;
			}

			// Separator to show a different item group
			ImGui::Separator();

			// Exit the game
			if (ImGui::MenuItem("Exit"))
			{
				m_engine->Exit();
			}

			// Pop the menu
			ImGui::EndMenu();
		}

		// Push a new menu
		if (ImGui::BeginMenu("Edit"))
		{
			// Check if there is anything to undo
			if (m_actions.size)
			{
				// If so, make it clickable
				if (ImGui::MenuItem("Undo", "CTRL+Z"))
				{
					Undo_Action();
				}
			}

			// If there is nothing to undo, disable the button
			else
			{
				ImGui::MenuItem("Undo", "CTRL+Z", false, false);
			}
			
			// Check if it is safe to redo
			if (m_actions.history.size() != m_actions.size)
			{
				// If so, make it clickable
				if (ImGui::MenuItem("Redo", "CTRL+Y"))
				{
					Redo_Action();
				}
			}

			// If there is nothing to redo, disable it
			else
			{
				ImGui::MenuItem("Redo", "CTRL+Y", false, false);
			}

			// Pop the menu
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("GameObject##menuBar"))
		{
			if (ImGui::MenuItem("Save PreFab##menuBar"))
			{
				if (m_selected_object.IsValid())
				{
					SavePrefab(m_selected_object);
				}
			}

			if (ImGui::MenuItem("Load PreFab##menuBar"))
			{
				LoadPreFab(this);
			}

			// Add Component Buttons
			if (ImGui::BeginMenu("Components"))
			{
				// Everything has a transform
				// Everything is going to have a data component

				if (ImGui::MenuItem("RigidBody"))
				{
					if (m_selected_object.IsValid())
					{
						if (m_selected_object.GetComponent<RigidBodyComponent>().IsValid())
						{
							HAS_COMPONENT_Editor;
						}
						else
						{
							m_selected_object.AddComponent<RigidBodyComponent>();
							Push_AddComponent_Editor(RigidBodyComponent);
						}
					}
				}
				if (ImGui::MenuItem("Dynamic Collider"))
				{
					if (m_selected_object.IsValid())
					{
						if (m_selected_object.GetComponent<DynamicCollider2DComponent>().IsValid())
						{
							HAS_COMPONENT_Editor;
						}
						else
						{
							if (m_selected_object.GetComponent<StaticCollider2DComponent>().IsValid())
							{
								AddPopUp(PopUpWindow(ErrorList[HasStaticCollider], 2.0f, PopUpPosition::Mouse));
							}
							else
							{
								bool added_rigidbody = false;

								m_selected_object.AddComponent<DynamicCollider2DComponent>();
								if (!m_selected_object.GetComponent<RigidBodyComponent>().IsValid())
								{
									added_rigidbody = true;
									m_selected_object.AddComponent<RigidBodyComponent>();
									AddPopUp(PopUpWindow("Added a RigidBody Component.", 1.5f, PopUpPosition::Mouse));
								}

								Push_Action({ 0, added_rigidbody, nullptr,{ m_selected_object.Getid(), true }, Action_AddComponent_DynamicCollider });
							}
						}
					}
				}
				if (ImGui::MenuItem("Static Collider"))
				{
					if (m_selected_object.IsValid())
					{
						if (m_selected_object.GetComponent<StaticCollider2DComponent>().IsValid())
						{
							HAS_COMPONENT_Editor;
						}
						else
						{
							if (m_selected_object.GetComponent<DynamicCollider2DComponent>().IsValid() || m_selected_object.GetComponent<RigidBodyComponent>().IsValid())
							{
								AddPopUp(PopUpWindow(ErrorList[HasRigidBodyDynamicCollider], 2.0f, PopUpPosition::Mouse));
							}
							else
							{
								m_selected_object.AddComponent<StaticCollider2DComponent>(glm::vec3(1, 1, 0), collisionLayers::allCollision, Collider2D::colliderType::colliderBox);
								Push_AddComponent_Editor(StaticCollider2DComponent);
							}
						}
					}
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Script"))
				{
					if (m_selected_object.IsValid())
					{
						if (m_selected_object.GetComponent<ScriptComponent>().IsValid())
						{
							HAS_COMPONENT_Editor;
						}
						else
						{
							m_selected_object.AddComponent<ScriptComponent>();
							Push_AddComponent_Editor(ScriptComponent);
						}
					}
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Sprite"))
				{
					if (m_selected_object.IsValid())
					{
						if (m_selected_object.GetComponent<SpriteComponent>().IsValid())
						{
							HAS_COMPONENT_Editor;
						}
						else
						{
							m_selected_object.AddComponent<SpriteComponent>();
							Push_AddComponent_Editor(SpriteComponent);
						}
					}
				}

				if (ImGui::MenuItem("Particle System"))
				{
					if (m_selected_object.IsValid())
					{
						if (m_selected_object.GetComponent<ParticleSystem>().IsValid())
						{
							HAS_COMPONENT_Editor;
						}
						else
						{
							m_selected_object.AddComponent<ParticleSystem>();
							Push_AddComponent_Editor(ParticleSystem);
						}
					}
				}
				if (ImGui::MenuItem("Camera"))
				{
					if (m_selected_object.IsValid())
					{
						if (m_selected_object.GetComponent<Camera>().IsValid())
						{
							HAS_COMPONENT_Editor;
						}
						else
						{
							m_selected_object.AddComponent<Camera>();
							Push_AddComponent_Editor(Camera);
						}
					}
				}
				if (ImGui::MenuItem("Background"))
				{
					if (m_selected_object.IsValid())
					{
						if (m_selected_object.GetComponent<BackgroundComponent>().IsValid())
						{
							HAS_COMPONENT_Editor;
						}
						else
						{
							m_selected_object.AddComponent<BackgroundComponent>();
							Push_AddComponent_Editor(BackgroundComponent);
						}
					}
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		// Push next menu
		if (ImGui::BeginMenu("Windows##menuBar"))
		{
			// Button Toggle for the Settings Panel
			if (ImGui::MenuItem("Settings", nullptr, m_editorState.settings))
			{
				m_editorState.settings = !m_editorState.settings;
			}

			// Button Toggle for the Objects List Panel
			if (ImGui::MenuItem("Object List", nullptr, m_editorState.objectList))
			{
				m_editorState.objectList = !m_editorState.objectList;
			}

			// Button Toggle for the Objects List Panel
			if (ImGui::MenuItem("Console", nullptr, m_editorState.console))
			{
				m_editorState.console = !m_editorState.console;
			}


			if (ImGui::MenuItem("Effects", nullptr, m_editorState.ppfx))
			{
				m_editorState.ppfx = !m_editorState.ppfx;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Scripting##menuBar"))
		{
			if (ImGui::MenuItem("Reload All Scripts"))
			{
				ScriptSystem::ReloadAll();
			}

			ImGui::EndMenu();
		}

		// Run the Save/Load update
		SaveLoad();

		ImGui::EndMainMenuBar();
	}
}


// Watches for Saving and Loading
void Editor::SaveLoad()
{
	// Check if we need to save
	if (m_editorState.fileSave)
	{
		if (m_editorState.fileOpened)
		{
			m_engine->FileSave(m_filename.c_str());

			// Tell the user we auto saved
			std::string saveMessage = "Saved file ";
			saveMessage += m_filename;
			AddPopUp(PopUpWindow(saveMessage.c_str(), 2.2f, PopUpPosition::BottomRight));
		}
		else
		{
			m_editorState.fileOpened = true;

			logger << "[EDITOR] Saving Level Dialog\n";
			SaveLevel();
		}

		m_editorState.fileDirty = false;
		m_editorState.fileSave = false;
	}

	// Check if we need to load
	if (m_editorState.fileLoad)
	{
		logger << "[EDITOR] Opening Level Dialog\n";
		OpenLevel();
		
		m_editorState.fileLoad = false;
		
		m_editorState.fileNewFile = true;
		m_editorState.fileOpened = true;

		Reload();
	}
}


void Editor::AutoSave(float dt)
{
	// Add the time to the timer
	m_editorState.saveTimer += dt;

	// Check if it is time to save
	if (m_editorState.saveTimer >= (m_editorState.saveInterval * 60.0f) && m_editorState.fileDirty == true)
	{
		// Using this to build a file name
		std::stringstream ss;

		// Write the timestamp and append _AutoSave.json
		ss << std::time(nullptr) << "_AutoSave.json";

		// Write the File
		m_engine->FileSaveCompact(ss.str().c_str());

		// Tell the user we auto saved
		AddPopUp(PopUpWindow("Auto Saved.", 2.2f, PopUpPosition::BottomRight));

		// Reset the timer
		m_editorState.saveTimer = 0.0f;

		// Nothing to save now
		m_editorState.fileDirty = false;
	}
}


std::string Editor::GetSaveTitle() const
{
	// Build a Title
	std::string title(" - ");

	// Append the current filename
	title += m_filename;

	// Check if any modifications needs to be saved
	if (m_editorState.fileDirty)
	{
		title += "*";
	}

	return title;
}


#ifdef _WIN32

// Windows Code for Opening a Level
void Editor::OpenLevel()
{
	// Save the path to the file
	char filename[MAX_PATH] = { 0 };

	// Struct to file before passing it to a function
	OPENFILENAME file;

	// Init the struct
	ZeroMemory(&file, sizeof(file));

	// Save the size of the struct
	file.lStructSize = sizeof(file);

	// Tell windows which window owns this action
	file.hwndOwner = glfwGetWin32Window(m_engine->GetWindow());

	// Filter out any files that don't fit this format
	file.lpstrFilter = "JSON\0*.json\0Any File\0*.*\0";

	// Pointer to the filename buffer
	file.lpstrFile = filename;

	// The cout of the buffer
	file.nMaxFile = MAX_PATH;

	// Window title of the dialog box
	file.lpstrFileTitle = "Load a level";

	// Flags to prevent openning non-existant files
	file.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	// Function Call to open the dialag box
	if (GetOpenFileName(&file))
	{
		// Pass the filename to the engine
		logger << "[EDITOR] Loading File: " << filename << "\n";
		m_engine->FileLoad(filename);

		m_filename = filename;
	}
	else
	{
		// Print out any errors.
		switch (CommDlgExtendedError())
		{
		case CDERR_DIALOGFAILURE:   Logging::Log("CDERR_DIALOGFAILURE\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case CDERR_FINDRESFAILURE:  Logging::Log("CDERR_FINDRESFAILURE\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case CDERR_INITIALIZATION:  Logging::Log("CDERR_INITIALIZATION\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case CDERR_LOADRESFAILURE:  Logging::Log("CDERR_LOADRESFAILURE\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case CDERR_LOADSTRFAILURE:  Logging::Log("CDERR_LOADSTRFAILURE\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case CDERR_LOCKRESFAILURE:  Logging::Log("CDERR_LOCKRESFAILURE\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case CDERR_MEMALLOCFAILURE: Logging::Log("CDERR_MEMALLOCFAILURE\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case CDERR_MEMLOCKFAILURE:  Logging::Log("CDERR_MEMLOCKFAILURE\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case CDERR_NOHINSTANCE:     Logging::Log("CDERR_NOHINSTANCE\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case CDERR_NOHOOK:          Logging::Log("CDERR_NOHOOK\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case CDERR_NOTEMPLATE:      Logging::Log("CDERR_NOTEMPLATE\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case CDERR_STRUCTSIZE:      Logging::Log("CDERR_STRUCTSIZE\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case FNERR_BUFFERTOOSMALL:  Logging::Log("FNERR_BUFFERTOOSMALL\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case FNERR_INVALIDFILENAME: Logging::Log("FNERR_INVALIDFILENAME\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case FNERR_SUBCLASSFAILURE: Logging::Log("FNERR_SUBCLASSFAILURE\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		default: Logging::Log("[EDITOR] User closed OpenLevel Dialog.");
		}
	}
}


// Windows Code for Opening a Level
void Editor::SaveLevel()
{
	// Save the path to the file
	char filename[MAX_PATH] = { 0 };

	// Struct to file before passing it to a function
	OPENFILENAME file;

	// Init the struct
	ZeroMemory(&file, sizeof(file));

	// Save the size of the struct
	file.lStructSize = sizeof(file);

	// Tell windows which window owns this action
	file.hwndOwner = glfwGetWin32Window(m_engine->GetWindow());

	// Filter out any files that don't fit this format
	file.lpstrFilter = "JSON\0*.json\0Any File\0*.*\0";

	// Pointer to the filename buffer
	file.lpstrFile = filename;

	// The cout of the buffer
	file.nMaxFile = MAX_PATH;

	// Window title of the dialog box
	file.lpstrFileTitle = "Load a level";

	// Flags to prevent openning non-existant files
	file.Flags = OFN_DONTADDTORECENT;

	// Function Call to open the dialag box
	if (GetSaveFileName(&file))
	{
		// Log and load the file in the engine
		logger << "[EDITOR] Saving File: " << filename << "\n";
		m_filename = filename;

		if (m_filename.find(".json") == std::string::npos)
		{
			m_filename += ".json";
		}

		m_engine->FileSave(m_filename.c_str());
	}
	else
	{
		// Print out any errors.
		switch (CommDlgExtendedError())
		{
		case CDERR_DIALOGFAILURE:   Logging::Log("CDERR_DIALOGFAILURE\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case CDERR_FINDRESFAILURE:  Logging::Log("CDERR_FINDRESFAILURE\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case CDERR_INITIALIZATION:  Logging::Log("CDERR_INITIALIZATION\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case CDERR_LOADRESFAILURE:  Logging::Log("CDERR_LOADRESFAILURE\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case CDERR_LOADSTRFAILURE:  Logging::Log("CDERR_LOADSTRFAILURE\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case CDERR_LOCKRESFAILURE:  Logging::Log("CDERR_LOCKRESFAILURE\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case CDERR_MEMALLOCFAILURE: Logging::Log("CDERR_MEMALLOCFAILURE\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case CDERR_MEMLOCKFAILURE:  Logging::Log("CDERR_MEMLOCKFAILURE\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case CDERR_NOHINSTANCE:     Logging::Log("CDERR_NOHINSTANCE\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case CDERR_NOHOOK:          Logging::Log("CDERR_NOHOOK\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case CDERR_NOTEMPLATE:      Logging::Log("CDERR_NOTEMPLATE\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case CDERR_STRUCTSIZE:      Logging::Log("CDERR_STRUCTSIZE\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case FNERR_BUFFERTOOSMALL:  Logging::Log("FNERR_BUFFERTOOSMALL\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case FNERR_INVALIDFILENAME: Logging::Log("FNERR_INVALIDFILENAME\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		case FNERR_SUBCLASSFAILURE: Logging::Log("FNERR_SUBCLASSFAILURE\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); AddPopUp(PopUpWindow(ErrorList[OpenFileError], 2.0f, PopUpPosition::Mouse));  break;
		default: Logging::Log("[EDITOR] User closed OpenLevel Dialog.");
		}
	}
}

#endif


static unsigned long long FileTimeToInt64(const FILETIME & ft) 
{
	return (static_cast<unsigned long long>(ft.dwHighDateTime) << 32) | static_cast<unsigned long long>(ft.dwLowDateTime); 
}


static float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks)
{
	static unsigned long long _previousTotalTicks = 0;
	static unsigned long long _previousIdleTicks = 0;

	unsigned long long totalTicksSinceLastTime = totalTicks - _previousTotalTicks;
	unsigned long long idleTicksSinceLastTime = idleTicks - _previousIdleTicks;

	float ret = 1.0f - ((totalTicksSinceLastTime > 0) ? static_cast<float>(idleTicksSinceLastTime) / totalTicksSinceLastTime : 0);

	_previousTotalTicks = totalTicks;
	_previousIdleTicks = idleTicks;
	return ret;
}


float GetCPULoad()
{
	FILETIME idleTime, kernelTime, userTime;
	return GetSystemTimes(&idleTime, &kernelTime, &userTime) ? CalculateCPULoad(FileTimeToInt64(idleTime), FileTimeToInt64(kernelTime) + FileTimeToInt64(userTime)) : -1.0f;
}


// Settings Panel in the editor
//     Houses a bunch of useful stuff, needs some UI/UX work
void Editor::SettingsPanel(float dt)
{
	using namespace ImGui;

	// Timer for CPU usage
	static float timer = 0.0f;

	// Setup the size and Position of this window
	SetNextWindowSize(ImVec2(globalScale * 250, globalScale * 400), ImGuiCond_Once);
	SetNextWindowPos(ImVec2(0, 432), ImGuiCond_Once);

	// Start the Settings panel
	Begin("Settings", nullptr);

	// Struct to get the memory usage
	PROCESS_MEMORY_COUNTERS pmc;

	// Get the memory info
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));

	// Save the total memory used by this process
	SIZE_T virtualMemUsedByMe = pmc.WorkingSetSize;

	// Print out the data
	ImGui::Text("RAM: %u MB", virtualMemUsedByMe / (1024 * 1024));
	ImGui::Text("CPU: %f%%", m_cpu_load[0]);
	
	// Add time to the timer
	timer += dt;

	// Check if we should read the CPU load
	if (timer > 0.75f)
	{
		// Push to the back but shift everything forward one
		m_cpu_load.push_back_pop_front(GetCPULoad() * 100.0f);

		// set a new cpu peak
		if (m_cpu_peak < m_cpu_load[m_cpu_load.m_size - 1])
		{
			m_cpu_peak = m_cpu_load[m_cpu_load.m_size - 1];
		}

		// reset the timer
		timer = 0.0f;
	}

	// Define here if need to change all buttons sizes in this panel
#define SETTINGS_BUTTON_SIZE 

	// Console Button Toggle
	if (ImGui::Button("Console" SETTINGS_BUTTON_SIZE))
	{
		m_editorState.console = !m_editorState.console;
	}

	// Separates groups
	ImGui::Separator();

	// Save the current data
	if (Button("Save##editor_settings" SETTINGS_BUTTON_SIZE))
	{
		m_editorState.fileSave = true;
	}
	SameLine();

	// Load a file
	if (Button("Load##editor_settings" SETTINGS_BUTTON_SIZE))
	{
		m_editorState.fileLoad = true;
	}

	// Allow users to change Save Interval	
	PushItemWidth(50);
	InputFloat("Save Time (mins)##edtor_save_interval", &m_editorState.saveInterval, 0, 0, 2);
	PopItemWidth();


	// New Group
	ImGui::Separator();

	// Freeze/Unfreeze the game
	if (Button("Play/Pause##editor_panel" SETTINGS_BUTTON_SIZE))
	{
		m_editorState.freeze = !m_editorState.freeze;
	}
	SameLine();

	// Reload the lua scripts
	if (ImGui::Button("Reload" SETTINGS_BUTTON_SIZE))
	{

	}

	if (ImGui::Button("Use Editor Camera"))
	{
		Push_Action({ Camera::GetActiveCamera(), m_editor_cam, nullptr, { 0, false },
			[](EditorAction& a)
		{
			Camera *prev = a.save.GetData<Camera *>();
			Camera *curr = a.current.GetData<Camera *>();

			if (a.redo)
			{
				curr->Use();
			}
			else
			{
				prev->Use();
			}
		}
		});

		m_editor_cam->Use();
	}

	ImGui::PushItemWidth(110);
	ImGui::DragFloat("Camera Speed", &m_editorSettings.cameraSpeed, (1 / 16.0f), 0.0f, FLT_MAX, "%.1f");
	ImGui::DragFloat("Camera Arrow Speed", &m_editorSettings.cameraArrowSpeed, (1 / 16.0f), 0.0f, FLT_MAX, "%.1f");
	ImGui::DragFloat("Camera Zoom",  &m_editorSettings.cameraZoom,  (1 / 16.0f), 0.0f, FLT_MAX, "%.1f");
	ImGui::PopItemWidth();

	// Snapping settings.
	ImGui::Separator();
	ImGui::PushItemWidth(110);
	ImGui::Checkbox("Snap Enabled", &m_editorSettings.snap);
	ImGui::Checkbox("Absolute Snap", &m_editorSettings.absoluteSnap);
	ImGui::InputFloat("Snap Interval", &m_editorSettings.snapInterval, 0.25f, 1.0f);
	ImGui::InputFloat("Rotation Interval", &m_editorSettings.rotationSnapInterval, 15, 30);
	ImGui::PopItemWidth();

	// Misc settings.
	ImGui::Separator();
	ImGui::PushItemWidth(110);
	ImGui::Checkbox("Transform Tools Select", &m_editorSettings.selectWithTransformTools);
	ImGui::Checkbox("Info on Window", &m_editorSettings.infoOnTitleBar);

	ImGui::Separator();
	// Pulls up the Editor Keybinds
	if (ImGui::Button("Keybinds" SETTINGS_BUTTON_SIZE))
	{
		
	}
	SameLine();

	// Pulls up the style editor
	if (ImGui::Button("Style Editor" SETTINGS_BUTTON_SIZE))
	{
		OpenPopup("##editor_settings_style");
	}

	// ImGui Popup for changing the style
	if (ImGui::BeginPopup("##editor_settings_style"))
	{
		ImGui::ShowStyleEditor();

		EndPopup();
	}

	End();
}


// Text feedback popup -- display command matches to current input
bool Editor::Console::PopUp(ImVec2& pos, ImVec2& size)
{
	// Make sure the popup needs to be seen
	if (!m_state.m_popUp || !m_matches.size())
		return false;

	// Flags for the dummy window
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
	ImGui::SetNextWindowSize(ImVec2(globalScale * size.x, globalScale * size.y));

	// Dummy Window, used to add things to
	ImGui::Begin("console_popup##console_dummy_window", nullptr, flags);
	
	// Prevent the keyboard here
	ImGui::PushAllowKeyboardFocus(false);

	// Go through the matches found from the input text
	for (auto i = 0; i < m_matches.size(); ++i)
	{
		// Check if the item is the hovered or active for colors
		bool isActiveIndex = m_state.activeIndex == i;

		// Check if we are at the active index
		if (isActiveIndex)
		{
			// Push a different style for the active one
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(HexVecA(0x072f70FF)));
		}
		
		// Display the options as a selectable text button
		ImGui::PushID(i);
		if (ImGui::Selectable(m_matches[i], m_state.activeIndex))
		{
			m_state.clickedIndex = i;
		}
		ImGui::PopID();

		// Check if the current hovered/selected is the index
		if (isActiveIndex)
		{
			// Mark that one has been hovered
			if (m_state.m_selection_change)
			{
				ImGui::SetScrollHere();
				m_state.m_selection_change = false;
			}

			// Pop the style pushed
			ImGui::PopStyleColor(1);
		}
	}

	// Check if this popup has focus
	bool isFocuesed = ImGui::IsRootWindowFocused();

	// Allow keyboard again
	ImGui::PopAllowKeyboardFocus();

	// Pop the window
	ImGui::End();

	// Pop the style
	ImGui::PopStyleVar(1);

	// Return the focus variable
	return isFocuesed;
}


// Register a command with the Console of the Editor
void Editor::RegisterCommand(const char *command, std::function<void()>&& f)
{
	m_Console.RegisterCommand(command, std::forward<std::function<void()>>(f));
}


// Register a command using a lambda
void Editor::Console::RegisterCommand(const char *command, std::function<void()>&& f)
{
	// Hash the string and save the command 
	m_commands.emplace(hash(command), Command(command, strlen(command), f));
}


// ImGui interface with console data
void Editor::Console::Draw()
{
	// Setup a char * buffer and tell imgui to draw the console the first time in the center
	char command_buffer[1024] = { 0 };

	// Center Console the first time it is created
	ImGui::SetNextWindowPosCenter(ImGuiSetCond_FirstUseEver);

	// Setup the console window flags
	int winflags = 0;
	if (m_state.m_popUp)
	{
		winflags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	}

	// Set the size once
	ImGui::SetNextWindowSize(ImVec2(globalScale * 800, globalScale * 550), ImGuiSetCond_FirstUseEver);

	// Push the window
	ImGui::Begin("Console", nullptr, winflags);

	// Clear the Buffer data
	if (ImGui::Button("Clear Log"))
	{
		Clear();
	}

	// Draw button on same line as previous element
	ImGui::SameLine();

	// Push Style
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));

	// Copy the Log to a clipboard
	if (ImGui::Button("Copy"))
	{
		ImGui::LogToClipboard();
	}

	// Pop Style
	ImGui::PopStyleVar();
	
	ImGui::SameLine();

	// Draw the filter
	m_log_filter.Draw("Filter", -100.0f);

	// The log box itself
	ImVec2 size = ImGui::GetWindowSize();
	ImGui::BeginChild("scrolling", ImVec2(size.x * 0.95f, size.y * 0.8f));

	// Check if any filters are active and need to be applied
	if (m_log_filter.IsActive())
	{
		// Get the start of the data
		const char *buf_begin = m_log_buffer.begin();
		const char *line = buf_begin;

		// Go through the data and 
		for (int line_no = 0; line != nullptr; line_no++)
		{
			// Get the data
			const char *line_end = (line_no < m_offsets.Size) ? buf_begin + m_offsets[line_no] : nullptr;

			// Filter the text
			if (m_log_filter.PassFilter(line, line_end))
			{
				ImGui::TextUnformatted(line, line_end);
			}

			line = line_end && line_end[1] ? line_end + 1 : nullptr;
		}
	}
	else
	{
		// Put the text out without a filter
		ImGui::TextUnformatted(m_log_buffer.begin());
	}


	// Scroll down in the console
	if (m_scroll)
	{
		ImGui::SetScrollHere(1.0f);
	}
	m_scroll = false;

	ImGui::EndChild();
	ImGui::Separator();



	ImGui::Text("Command:");
	ImGui::SameLine();

	int flags = ImGuiInputTextFlags_EnterReturnsTrue     |
				 ImGuiWindowFlags_NoSavedSettings         |
				 ImGuiInputTextFlags_CallbackAlways       |
				 ImGuiInputTextFlags_CallbackCharFilter   |
				 ImGuiInputTextFlags_CallbackCompletion   |
				 ImGuiInputTextFlags_CallbackHistory;

	// Input into the console buffer
	if (ImGui::InputText("", command_buffer, sizeof(command_buffer), flags, Input_Callback, this))
	{
		// Get the line data and save it for parameters
		m_line = command_buffer;

		// Make sure there is data in the line
		if (m_line.size() > 0)
		{
			// Skip leading spaces
			auto first_of_not_space = m_line.find_first_not_of(' ');

			// Extract the command from the line
			std::string command = m_line.substr(first_of_not_space, m_line.find_first_of(' '));

			// Make Everything uppercase
			std::transform(command.begin(), command.end(), command.begin(), ::tolower);

			// Get the hash of the command
			size_t str_hash = hash(command.c_str());

			// Log the command and display it on screen
			if (first_of_not_space != std::string::npos)
			{
				m_line = m_line.substr(first_of_not_space);

				// Add the log to the buffer
				Log(m_line.c_str());

				// Save the command for history
				if (m_log_history.size() < 100)
				{
					m_log_history.emplace_back(m_line.c_str());
				}
			}

			// Check if the popup was clicked and copy the data from the clicked item to here
			if (m_state.m_popUp && m_state.clickedIndex != -1)
			{
				if (m_commands.find(str_hash) != m_commands.end())
				{
					memmove(command_buffer, m_commands[str_hash].command, m_commands[str_hash].cmd_length);
				}
			}
			else
			{
				if (m_commands.find(str_hash) != m_commands.end())
				{
					m_commands.find(str_hash)->second.func();
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
		// Puts keyboard focus back to the text input
		ImGui::SetKeyboardFocusHere(-1);
		m_state.m_popUp = false;
	}

	if ((ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
	{
		ImGui::SetKeyboardFocusHere(-1);
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


// Clear the Log Buffers inside the console
void Editor::Console::Clear()
{
	// Check if we need to clear anything first
	if (!m_log_buffer.empty())
	{
		m_log_buffer.clear();
	}
}


///
// Post Processing Effect Window (Author: Max Rauffer)
///
#include "graphics\Screen.h"
static std::vector<char*> layerNames;
static std::vector<char*> addButtonNames;
static std::vector<char*> removeButtonNames;
static std::vector<char*> layerEffectNames;
static std::vector<char*> moveUpButtonNames;
static std::vector<char*> moveDownButtonNames;

void Editor::PPFX()
{
	using namespace ImGui;
	SetNextWindowSize(ImVec2(globalScale * 310, globalScale * 400));
	SetNextWindowPos(ImVec2(280, 30), ImGuiCond_Once);
	Begin("Post Processing", nullptr, ImGuiWindowFlags_NoResize);


	int i = 0;
	int j = 0;
	for (auto layer : Screen::GetLayerList())
	{
		while (i >= layerNames.size())
			layerNames.push_back(new char[64]);

		sprintf(layerNames[i], "Layer %d##ppfxlayer%d", layer->GetLayer(), layer->GetLayer());

		if (CollapsingHeader(layerNames[i]))
		{
			while (i >= addButtonNames.size()) 
				addButtonNames.push_back(new char[64]);

			sprintf(addButtonNames[i], "Add Effect##ppfxlayer%dadd", layer->GetLayer());
			if (Button(addButtonNames[i]))
			{
				engine->GetFXManager()->AddEffect(layer->GetLayer(), FX::DEFAULT);
			}

			int count = 0;
			auto list = layer->GetFXList(); // Copy list to avoid breaking after editting list
			bool editted = false; // true for rest of update after editting list length to avoid going out of bounds
			for (FX& fx : list)
			{
				while (j >= layerEffectNames.size())
					layerEffectNames.push_back(new char[64]);

				sprintf(layerEffectNames[j], "###ppfxlayer%deffect%d", layer->GetLayer(), j);

				PushItemWidth(150);
				if(!editted)
					Combo(layerEffectNames[j], reinterpret_cast<int*>(&layer->GetFXList()[count]), FX_Names, _countof(FX_Names));
				else
					Combo(layerEffectNames[j], reinterpret_cast<int*>(&fx), FX_Names, _countof(FX_Names)); // Give temp value to avoid out of bounds
				PopItemWidth();

				// Remove
				SameLine();
				while (j >= removeButtonNames.size())
					removeButtonNames.push_back(new char[64]);

				sprintf(removeButtonNames[j], "Remove##ppfxlayer%dremove%d", layer->GetLayer(), j);
				if (Button(removeButtonNames[j]))
				{
					layer->GetFXList().erase(layer->GetFXList().begin() + count);
					editted = true;
				}

				// Move Up
				SameLine();
				if (count != 0)
				{
					while (j >= moveUpButtonNames.size())
						moveUpButtonNames.push_back(new char[64]);

					sprintf(moveUpButtonNames[j], "^##ppfxlayer%dup%d", layer->GetLayer(), j);
					if (Button(moveUpButtonNames[j]))
					{
						std::vector<FX> newFX;
						bool added = false;
						for (int f = 0; f < layer->GetFXList().size(); f++)
						{
							if (f == count - 1 && !added)
							{
								newFX.push_back(fx);
								added = true;
								f--;
							}
							else if(f != count) // Dont add this one again
								newFX.push_back(layer->GetFXList()[f]);
						}
						layer->GetFXList() = newFX;
					}
				}
				else
					Button(" ");

				// Move Down
				SameLine();
				if (count != layer->GetFXList().size() - 1)
				{
					while (j >= moveDownButtonNames.size())
						moveDownButtonNames.push_back(new char[64]);

					sprintf(moveDownButtonNames[j], "v##ppfxlayer%ddown%d", layer->GetLayer(), j);
					if (Button(moveDownButtonNames[j]))
					{
						std::vector<FX> newFX;
						bool added = false;
						for (int f = 0; f < layer->GetFXList().size(); f++)
						{
							if (f == count)
							{
								newFX.push_back(layer->GetFXList()[f + 1]);
								newFX.push_back(fx);
								f++;
							}
							else
								newFX.push_back(layer->GetFXList()[f]);
						}
						layer->GetFXList() = newFX;
					}
				}
				else
					Button(" ");

				j++;
				count++;
			}
		}

		++i;
	}
	

	End();
}

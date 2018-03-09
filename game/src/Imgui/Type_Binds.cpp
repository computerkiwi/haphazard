/*
FILE: Type_Binds.cpp
PRIMARY AUTHOR: Sweet

Copyright (C) 2017 DigiPen (USA) Corporation.
*/

// Main Includes
#include "Editor.h"
#include "Type_Binds.h"

// Components
#include "GameObjectSystem/ObjectInfo.h"
#include "Engine/Physics/RigidBody.h"
#include "graphics/SpriteComponent.h"
#include "graphics/Particles.h"
#include "Engine/Physics/Collider2D.h"
#include "Scripting/ScriptComponent.h"
#include "graphics/Camera.h"
#include "graphics/Background.h"

// Scripts
#include "EditorScript.h"

// GameObject, Spaces
#include "GameObjectSystem/GameObject.h"
#include "GameObjectSystem/GameSpace.h"

// Draw debug shapes
#include "graphics/DebugGraphic.h"
#include "graphics/Text.h"

// Keypress and Mouse
#include "Input/Input.h"

#include <cfloat>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#ifdef _WIN32
	#include <Windows.h>
	#include <psapi.h>
#endif

using namespace ImGui;





#define GAMEOBJECT_WINDOW_SIZE ImVec2(460, 744)
#define GAMEOBJECT_WINDOW_POS  ImVec2(1465, 30)


#define SPRITE_SELECTED_COLOR    ImVec4(223 / 255.0f, 104 / 255.0f, 76 / 255.0f, 1.0f)
#define SPRITE_ASSETS_LIST_SIZE  ImVec2(0, 250)




template <class Component, typename T>
void Action_General(EditorAction& a)
{
	ComponentHandle<Component> handle(a.handle);
	meta::Any obj(handle.Get());

	if (handle.GetGameObject().IsValid())
	{
		if (a.redo)
		{
			obj.SetPointerMember(a.name, a.current.GetData<T>());
		}
		else
		{
			obj.SetPointerMember(a.name, a.save.GetData<T>());
		}
	}
}


template <>
void Action_General<SpriteComponent, ResourceID>(EditorAction& a)
{
	ComponentHandle<SpriteComponent> handle(a.handle);

	if (handle.GetGameObject().IsValid())
	{
		if (a.redo)
		{
			handle->SetTextureID(a.current.GetData<ResourceID>());
		}
		else
		{
			handle->SetTextureID(a.save.GetData<ResourceID>());
		}
	}
}


template <typename T>
void Action_General_Particle(EditorAction& a)
{
	ComponentHandle<ParticleSystem> handle(a.handle);
	meta::Any obj(handle->GetSettings());

	if (handle.GetGameObject().IsValid())
	{
		if (a.redo)
		{
			obj.SetPointerMember(a.name, a.current.GetData<T>());
		}
		else
		{
			obj.SetPointerMember(a.name, a.save.GetData<T>());
		}
	}
}


void Action_General_TagsAdd(EditorAction& a)
{
	ComponentHandle<ObjectInfo> handle(a.handle);

	if (handle.GetGameObject().IsValid())
	{
		if (a.redo)
		{
			handle->AddTag(a.current.GetData<std::string>().c_str());
		}
		else
		{
			handle->m_tags.erase(a.save.GetData<std::size_t>());
		}
	}
}


void Action_General_TagsRemove(EditorAction& a)
{
	ComponentHandle<ObjectInfo> handle(a.handle);

	if (handle.GetGameObject().IsValid())
	{
		if (a.redo)
		{
			handle->m_tags.erase(a.save.GetData<std::size_t>());
		}
		else
		{
			handle->AddTag(a.current.GetData<std::string>().c_str());
		}
	}
}


template <class Collider, typename T>
void Action_General_Collider(EditorAction& a)
{
	ComponentHandle<Collider> handle(a.handle);

	meta::Any colliderData(&handle->ColliderData());

	if (handle.GetGameObject().IsValid())
	{
		if (a.redo)
		{
			colliderData.SetPointerMember(a.name, a.current.GetData<T>());
		}
		else
		{
			colliderData.SetPointerMember(a.name, a.save.GetData<T>());
		}
	}
}


template <class Component>
void Action_DeleteComponent(EditorAction& a)
{
	ComponentHandle<Component> handle(a.handle);

	if (handle.GetGameObject().IsValid())
	{
		if (a.redo)
		{
			handle.GetGameObject().DeleteComponent<Component>();
		}
		else
		{
			handle.GetGameObject().AddComponent<Component>(std::move(a.save.GetData<Component>()));
		}
	}
}


void Action_General_GameObjectDelete(EditorAction& a)
{
	GameObject object = a.save.GetData<GameObject>();

	if (object.IsValid())
	{
		if (a.redo)
		{
			//engine->GetEditor()->GetDeletedObjects().Duplicate(object);
			object.Delete();
		}
		else
		{
			// Duplicate From Space Function
			//object.GetSpace().DuplicateFromSpace(object, engine->GetEditor()->GetDeletedObjects());
		}
	}
}


const char * ErrorList[] = 
{
	"Error 00: See Max if you see this",
	"Error 01: Cannot Start the Editor",
	"Error 02: Already has this Component Type.",
	"Error 03: This Object has a RigidBody or a Dynamic Collider.",
	"Error 04: This Object has a Static Collider.",
	"Error 04: Unable to Open File. Check Log.",
	"Error 05: ",
	"Error 06: ",
	"Error 07: ",
	"Error 08: ",
	"Error 09: ",
	"Error 10: "
};


// Wrapper around bool, here so the bool has a default value
struct EditorBoolWrapper
{
	bool value = false;
	operator bool() const { return value; }
	EditorBoolWrapper& operator=(bool val) { value = val; return *this; }
};


// Used to Save the bool of whether the collider dimensions should match the transform scale
typedef std::map<GameObject_ID, EditorBoolWrapper> MatchScaleBool;
MatchScaleBool ObjectsMatchScale;

// Used to find if a drag sliders was clicked and released
typedef std::map<const char *, EditorBoolWrapper> ClickedList;
ClickedList widget_click;

// DynamicCollider Action
void Action_AddComponent_DynamicCollider(EditorAction& a);


const char * const ColliderTypeNames[] =
{
	"Solid",
	"Passthrough",
	"One Way"
};



// Button to release the drag sliders
#define Drag_Key Key::Mouse_1

// Setups the detection of the drag slider has been clicked
// Saves a float in a temporary value then stores the new value and the saved value
// Undo pops the saved value
// Redo pops the new value
#define Drag(NAME, SAVE, ITEM)																						 \
	if (DragFloat_ReturnOnClick(NAME, &ITEM, SLIDER_STEP))															 \
	{																												 \
		if (widget_click[#SAVE] == false)																			 \
		{																											 \
			SAVE = ITEM;																							 \
			widget_click[#SAVE] = true;																				 \
		}																											 \
	}																										 

// Setups the detection of the drag slider has been clicked
//     Saves a float in a temporary value then stores the new value and the saved value
//     Undo pops the saved value
//     Redo pops the new value
// Saves Anything instead of just a float -- used for glm::vecX
//    This version is used since the meta system does not register
//    parts of glm::vec2 or similar objects
#define Drag_Vec(NAME, SAVE, ITEM, VEC)																				 \
	if (DragFloat_ReturnOnClick(NAME, &ITEM, SLIDER_STEP))															 \
	{																												 \
		if (widget_click[#SAVE] == false)																			 \
		{																											 \
			SAVE = VEC;																								 \
			widget_click[#SAVE] = true;																				 \
		}																											 \
	}

// Setups the detection of the drag slider has been clicked
//     Saves a float in a temporary value then stores the new value and the saved value
//     Undo pops the saved value
//     Redo pops the new value
// Saves Anything instead of just a float -- used for glm::vecX
//    This version is used since the meta system does not register
//    parts of glm::vec2 or similar objects
//	  Min and Max
#define Drag_Vec_MinMax(NAME, SAVE, ITEM, VEC, MIN, MAX)															 \
	if (DragFloat_ReturnOnClick(NAME, &ITEM, SLIDER_STEP, MIN, MAX))												 \
	{																												 \
		if (widget_click[#SAVE] == false)																			 \
		{																											 \
			SAVE = VEC;																								 \
			widget_click[#SAVE] = true;																				 \
		}																											 \
	}

// Setups the detection of the drag slider has been clicked
//     Saves a float in a temporary value then stores the new value and the saved value
//     Undo pops the saved value
//     Redo pops the new value
//     Change Speed
#define Drag_Float_Speed(NAME, SAVE, ITEM, SPEED)																	 \
	if (DragFloat_ReturnOnClick(NAME, &ITEM, SPEED))																 \
	{																												 \
		if (widget_click[#SAVE] == false)																			 \
		{																											 \
			SAVE = ITEM;																							 \
			widget_click[#SAVE] = true;																				 \
		}																											 \
	}

// Setups the detection of the drag slider has been clicked
//     Saves a float in a temporary value then stores the new value and the saved value
//     Undo pops the saved value
//     Redo pops the new value
//     Change Speed
//     Min and Max
#define Drag_Float_Speed_MinMax(NAME, SAVE, ITEM, SPEED, MIN, MAX)													 \
	if (DragFloat_ReturnOnClick(NAME, &ITEM, SPEED, MIN, MAX))														 \
	{																												 \
		if (widget_click[#SAVE] == false)																			 \
		{																											 \
			SAVE = ITEM;																							 \
			widget_click[#SAVE] = true;																				 \
		}																											 \
	}

// Setups the detection of the drag slider has been clicked
//     Saves a int in a temporary value then stores the new value and the saved value
//     Undo pops the saved value
//     Redo pops the new value
#define Drag_Int(NAME, SAVE, ITEM)																					 \
	if (DragInt_ReturnOnClick(NAME, &ITEM, SLIDER_STEP))															 \
	{																												 \
		if (widget_click[#SAVE] == false)																			 \
		{																											 \
			SAVE = ITEM;																							 \
			widget_click[#SAVE] = true;																				 \
		}																											 \
	}

// Setups the detection of the drag slider has been clicked
//     Saves a int in a temporary value then stores the new value and the saved value
//     Undo pops the saved value
//     Redo pops the new value
//     Change Speed
#define Drag_Int_Speed(NAME, SAVE, ITEM, SPEED)																		 \
	if (DragInt_ReturnOnClick(NAME, &ITEM, SPEED))																	 \
	{																												 \
		if (widget_click[#SAVE] == false)																			 \
		{																											 \
			SAVE = ITEM;																							 \
			widget_click[#SAVE] = true;																				 \
		}																											 \
	}

// Setups the detection of the drag slider has been clicked
//     Saves a int in a temporary value then stores the new value and the saved value
//     Undo pops the saved value
//     Redo pops the new value
//     Change Speed
//     Min and Max
#define Drag_Int_Speed_MinMax(NAME, SAVE, ITEM, SPEED, MIN, MAX)													 \
	if (DragInt_ReturnOnClick(NAME, &ITEM, SPEED, MIN, MAX))														 \
	{																												 \
		if (widget_click[#SAVE] == false)																			 \
		{																											 \
			SAVE = ITEM;																							 \
			widget_click[#SAVE] = true;																				 \
		}																											 \
	}

// Mouse_1 (Drag_Key, defined at top) is released
// Pushes the action into the editor
#define DragRelease(COMPONENT, SAVE, ITEM, META_NAME)																 \
	if (Input::IsReleased(Drag_Key) && widget_click[#SAVE] == true)													 \
	{																												 \
		editor->Push_Action({ SAVE, ITEM,  META_NAME, handle, Action_General<COMPONENT, decltype(ITEM)> });			 \
		widget_click[#SAVE] = false;																				 \
	}

// Mouse_1 (Drag_Key, defined at top) is released
// Pushes the action into the editor
// Save the data, but pass it to a different Action_General function
#define DragRelease_Type(COMPONENT, SAVE, ITEM, META_NAME, TYPE)													 \
	if (Input::IsReleased(Drag_Key) && widget_click[#SAVE] == true)													 \
	{																												 \
		editor->Push_Action({ SAVE, ITEM,  META_NAME, handle, Action_General<COMPONENT, TYPE> });					 \
		widget_click[#SAVE] = false;																				 \
	}

// Mouse_1 (Drag_Key, defined at top) is released
// Pushes the action into the editor
// Cast SAVE and ITEM to TYPE then save them
#define DragRelease_Type_CastAll(COMPONENT, SAVE, ITEM, META_NAME, TYPE)											 \
	if (Input::IsReleased(Drag_Key) && widget_click[#SAVE] == true)													 \
	{																												 \
		editor->Push_Action({ TYPE(SAVE), TYPE(ITEM),  META_NAME, handle, Action_General<COMPONENT, TYPE> });		 \
		widget_click[#SAVE] = false;																				 \
	}

// Mouse_1 (Drag_Key, defined at top) is released
// Pushes the action into the editor
#define DragRelease_Func(COMPONENT, SAVE, ITEM, META_NAME, FUNC)																 \
	if (Input::IsReleased(Drag_Key) && widget_click[#SAVE] == true)													 \
	{																												 \
		editor->Push_Action({ SAVE, ITEM,  META_NAME, handle, FUNC });			 \
		widget_click[#SAVE] = false;																				 \
	}

// Transform Component Save Location
TransformComponent transformSave;

// RigidBody Save Location
RigidBodyComponent rigidBodySave;

// Collider Data Save Location
Collider2D         colliderSave;

// Sprite Save Location
struct SpriteSave
{
	int    AT_frame  = 0;
	float  AT_fps = 0;
	float  AT_timer  = 0;
	glm::vec4 m_Color = glm::vec4(1, 1, 1, 1);
	glm::vec2 m_TileAmount;
} spriteSave;

// Particles Save Location
ParticleSettings   particleSave;

// Camera Save
struct CameraSave
{
	//View matrix
	glm::vec3 m_Position = glm::vec3(0, 0, 5.0f);
	glm::vec3 m_Center = glm::vec3(0, 0, 0);
	glm::vec3 m_Up = glm::vec3(0, 1, 0);

	float m_Rotation = 0;

	//Projection matrix
	float m_Zoom = 3.0f;
	float m_AspectRatio;
	float m_Near = 1.0f;
	float m_Far = 10.0f;

	// Uniform buffer object location
	GLuint m_MatricesUbo;
} cameraSave;


// Background Component Save Location
struct BackgroundSave
{
	Texture* m_Texture;
	ResourceID m_resID;

	BACKGROUND_TYPE m_Type;
	
	glm::vec4 m_ParallaxBounds;
	
	glm::vec2 m_SubTextureSize = glm::vec2(0.1f, 1);
	glm::vec2 m_SubTexturePosition = glm::vec2(0, 0);
	
	glm::vec2 m_TextureXRange = glm::vec2(0, 1);
	glm::vec2 m_TextureYRange = glm::vec2(1, 0);

} bgSave;


bool Choose_Parent_ObjectList(Editor *editor, TransformComponent *transform, GameObject child)
{
	// Get all the names of the objects
	char name_buffer[128] = { 0 };
	GameObject object(0);

	for (auto& object_id : editor->m_objects)
	{
		// Use invalid gameObject as a delimiter
		if (object_id == INVALID_GAMEOBJECT_ID)
		{
			ImGui::Separator();
			continue;
		}
		object = object_id;
		std::string& name = object.GetComponent<ObjectInfo>().Get()->m_name;

		// You cannot parent yourself!
		if (child.Getid() == object_id)
		{
			continue;
		}

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

		// Draw each object
		if (ImGui::Selectable(name_buffer))
		{
			/*glm::vec2 parentPos = object.GetComponent<TransformComponent>()->GetPosition();
			glm::vec2 diff = transform->GetPosition() - parentPos;

			transform->SetPosition(diff);*/

			// It was clicked, Set the parent
			transform->SetParent(object);
			if (object.GetComponent<HierarchyComponent>().IsValid())
			{
				object.GetComponent<HierarchyComponent>()->Add(child);
			}
			else
			{
				object.AddComponent<HierarchyComponent>(child);
			}
			return true;
		}
	}

	return false;
}


#ifdef _WIN32

// Windows Code for Opening a Level
void LoadPreFab(Editor *editor)
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
	file.hwndOwner = glfwGetWin32Window(engine->GetWindow());

	// Filter out any files that don't fit this format
	file.lpstrFilter = "JSON\0*.json\0Any File\0*.*\0";

	// Pointer to the filename buffer
	file.lpstrFile = filename;

	// The cout of the buffer
	file.nMaxFile = MAX_PATH;

	// Window title of the dialog box
	file.lpstrFileTitle = "Load a level";

	// Flags to prevent openning non-existant files
	file.Flags = OFN_DONTADDTORECENT | OFN_NOCHANGEDIR;

	// Function Call to open the dialag box
	if (GetOpenFileName(&file))
	{
		// Log and load the file in the engine
		logger << "[EDITOR] Loading Prefab: " << filename << "\n";

		GameObject obj = GameObject::LoadPrefab(filename);
		obj.GetComponent<TransformComponent>()->SetPosition(editor->GetCamPos());
		editor->SetGameObject(obj);
	}
	else
	{
		// Print out any errors.
		switch (CommDlgExtendedError())
		{
		case CDERR_DIALOGFAILURE:   Logging::Log("CDERR_DIALOGFAILURE\n",   Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case CDERR_FINDRESFAILURE:  Logging::Log("CDERR_FINDRESFAILURE\n",  Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case CDERR_INITIALIZATION:  Logging::Log("CDERR_INITIALIZATION\n",  Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case CDERR_LOADRESFAILURE:  Logging::Log("CDERR_LOADRESFAILURE\n",  Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case CDERR_LOADSTRFAILURE:  Logging::Log("CDERR_LOADSTRFAILURE\n",  Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case CDERR_LOCKRESFAILURE:  Logging::Log("CDERR_LOCKRESFAILURE\n",  Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case CDERR_MEMALLOCFAILURE: Logging::Log("CDERR_MEMALLOCFAILURE\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case CDERR_MEMLOCKFAILURE:  Logging::Log("CDERR_MEMLOCKFAILURE\n",  Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case CDERR_NOHINSTANCE:     Logging::Log("CDERR_NOHINSTANCE\n",     Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case CDERR_NOHOOK:          Logging::Log("CDERR_NOHOOK\n",          Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case CDERR_NOTEMPLATE:      Logging::Log("CDERR_NOTEMPLATE\n",      Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case CDERR_STRUCTSIZE:      Logging::Log("CDERR_STRUCTSIZE\n",      Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case FNERR_BUFFERTOOSMALL:  Logging::Log("FNERR_BUFFERTOOSMALL\n",  Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case FNERR_INVALIDFILENAME: Logging::Log("FNERR_INVALIDFILENAME\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case FNERR_SUBCLASSFAILURE: Logging::Log("FNERR_SUBCLASSFAILURE\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		default: Logging::Log("[EDITOR] User closed OpenLevel Dialog.");
		}
	}
}


// Windows Code for Opening a Level
void SavePrefab(GameObject object)
{
	// Save the path to the file
	char filename[MAX_PATH] = { 0 };

	// Checks for file extensions
	std::string full_filename;

	// Struct to file before passing it to a function
	OPENFILENAME file;

	// Init the struct
	ZeroMemory(&file, sizeof(file));

	// Save the size of the struct
	file.lStructSize = sizeof(file);

	// Tell windows which window owns this action
	file.hwndOwner = glfwGetWin32Window(engine->GetWindow());

	// Filter out any files that don't fit this format
	file.lpstrFilter = "JSON\0*.json\0Any File\0*.*\0";

	// Pointer to the filename buffer
	file.lpstrFile = filename;

	// The cout of the buffer
	file.nMaxFile = MAX_PATH;

	// Window title of the dialog box
	file.lpstrFileTitle = "Load a level";

	// Flags to prevent openning non-existant files
	file.Flags = OFN_DONTADDTORECENT | OFN_NOCHANGEDIR;

	// Function Call to open the dialag box
	if (GetSaveFileName(&file))
	{
		// Log and load the file in the engine
		logger << "[EDITOR] Saving File: " << filename << "\n";
		full_filename = filename;

		if (full_filename.find(".json") == std::string::npos)
		{
			full_filename += ".json";
		}

		object.SaveToFile(full_filename.c_str());
	}
	else
	{
		// Print out any errors.
		switch (CommDlgExtendedError())
		{
		case CDERR_DIALOGFAILURE:   Logging::Log("CDERR_DIALOGFAILURE\n",   Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case CDERR_FINDRESFAILURE:  Logging::Log("CDERR_FINDRESFAILURE\n",  Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case CDERR_INITIALIZATION:  Logging::Log("CDERR_INITIALIZATION\n",  Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case CDERR_LOADRESFAILURE:  Logging::Log("CDERR_LOADRESFAILURE\n",  Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case CDERR_LOADSTRFAILURE:  Logging::Log("CDERR_LOADSTRFAILURE\n",  Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case CDERR_LOCKRESFAILURE:  Logging::Log("CDERR_LOCKRESFAILURE\n",  Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case CDERR_MEMALLOCFAILURE: Logging::Log("CDERR_MEMALLOCFAILURE\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case CDERR_MEMLOCKFAILURE:  Logging::Log("CDERR_MEMLOCKFAILURE\n",  Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case CDERR_NOHINSTANCE:     Logging::Log("CDERR_NOHINSTANCE\n",     Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case CDERR_NOHOOK:          Logging::Log("CDERR_NOHOOK\n",          Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case CDERR_NOTEMPLATE:      Logging::Log("CDERR_NOTEMPLATE\n",      Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case CDERR_STRUCTSIZE:      Logging::Log("CDERR_STRUCTSIZE\n",      Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case FNERR_BUFFERTOOSMALL:  Logging::Log("FNERR_BUFFERTOOSMALL\n",  Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case FNERR_INVALIDFILENAME: Logging::Log("FNERR_INVALIDFILENAME\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		case FNERR_SUBCLASSFAILURE: Logging::Log("FNERR_SUBCLASSFAILURE\n", Logging::Channel::CORE, Logging::Priority::CRITICAL_PRIORITY); break;
		default: Logging::Log("[EDITOR] User closed OpenLevel Dialog.");
		}
	}
}

#endif

static bool ObjectHasScript(ResourceID resource, std::vector<LuaScript>& scripts)
{
	for (auto& script : scripts)
	{
		if (script.GetResourceID() == resource)
		{
			return true;
		}
	}
	return false;
}

void ImGui_GameObject(GameObject object, Editor *editor)
{
	// Check if a nullptr was passed
	if (object && object.GetSpace())
	{
		// Display the object's id
		std::string name("GameObject - ");
		name += std::to_string(object.GetObject_id());
		name += "###GAMEOBJECT_ID";


		ImVec2 windowSize = GAMEOBJECT_WINDOW_SIZE;
		windowSize.x *= Editor::GetUiScale();
		windowSize.y *= Editor::GetUiScale();
		SetNextWindowSize(windowSize);
		SetNextWindowPos(GAMEOBJECT_WINDOW_POS, ImGuiCond_Once);
		Begin(name.c_str(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);

		// Setup style for buttons
		PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor(0.25f, 0.55f, 0.9f)));
		PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor(0.0f, 0.45f, 0.9f)));
		PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor(0.25f, 0.25f, 0.9f)));
		if (Button("Duplicate"))
		{
			object.Duplicate();
		}
		PopStyleColor(3);
		SameLine();
		if (Button("Delete"))
		{
			//editor->GetDeletedObjects().Duplicate(object);
			object.DeleteInternal();
			End();
			return;
		}

		if (Checkbox("Active##object_active", &object.GetComponent<ObjectInfo>()->m_active))
		{
			editor->Push_Action({ !object.GetComponent<ObjectInfo>()->m_active, object.GetComponent<ObjectInfo>()->m_active, "active", { object, true }, Action_General<ObjectInfo, bool> });
		}

		SameLine();

		if (Button("Edit Name##name_button"))
		{
			OpenPopup("Edit Name###name_popup");
		}

		if (BeginPopup("Edit Name###name_popup"))
		{
			char name_buffer[128] = { 0 };
			
			if (InputText("Edit Name##object_name_edit", name_buffer, sizeof(name_buffer), ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (name_buffer[0] != '\0')
				{
					editor->Push_Action({ object.GetComponent<ObjectInfo>()->m_name, std::string(name_buffer), "name", { object, true }, Action_General<ObjectInfo, std::string> });
					object.GetComponent<ObjectInfo>()->m_name = name_buffer;
				}
				CloseCurrentPopup();
			}
			EndPopup();
		}


		SameLine();

		// Add Component Buttons
		if (BeginPopup("Components"))
		{

			#define	COMPONENT_BUTTON_SIZE ImVec2(120, 0)

			// Everything has a transform
			// Everything is going to have a data component

			if (Button("RigidBody", COMPONENT_BUTTON_SIZE))
			{
				if (object.GetComponent<RigidBodyComponent>().IsValid())
				{
					HAS_COMPONENT;
				}
				else
				{
					if (object.GetComponent<StaticCollider2DComponent>().Get())
					{
						object.AddComponent<DynamicCollider2DComponent>(object.GetComponent<StaticCollider2DComponent>().Get());
						object.DeleteComponent<StaticCollider2DComponent>();
					}

					object.AddComponent<RigidBodyComponent>();
					Push_AddComponent(RigidBodyComponent);
				}
			}
			if (Button("Dynamic Collider", COMPONENT_BUTTON_SIZE))
			{
				if (object.GetComponent<DynamicCollider2DComponent>().IsValid())
				{
					HAS_COMPONENT;
				}
				else
				{
					if (object.GetComponent<StaticCollider2DComponent>().IsValid())
					{
						editor->AddPopUp(PopUpWindow(ErrorList[HasStaticCollider], 2.0f, PopUpPosition::Mouse));
					}
					else
					{
						bool added_rigidbody = false;

						object.AddComponent<DynamicCollider2DComponent>();
						if (!object.GetComponent<RigidBodyComponent>().IsValid())
						{
							added_rigidbody = true;
							object.AddComponent<RigidBodyComponent>();
							editor->AddPopUp(PopUpWindow("Added a RigidBody Component.", 1.5f, PopUpPosition::Mouse));
						}

						editor->Push_Action({ 0, added_rigidbody, nullptr,{ object.Getid(), true }, Action_AddComponent_DynamicCollider });
					}
				}
			}
			if (Button("Static Collider", COMPONENT_BUTTON_SIZE))
			{
				if (object.GetComponent<StaticCollider2DComponent>().IsValid())
				{
					HAS_COMPONENT;
				}
				else
				{
					if (object.GetComponent<DynamicCollider2DComponent>().IsValid() || object.GetComponent<RigidBodyComponent>().IsValid())
					{
						editor->AddPopUp(PopUpWindow(ErrorList[HasRigidBodyDynamicCollider], 2.0f, PopUpPosition::Mouse));
					}
					else
					{
						object.AddComponent<StaticCollider2DComponent>();
						Push_AddComponent(StaticCollider2DComponent);
					}
				}
			}
			Separator();
			if (Button("Script", COMPONENT_BUTTON_SIZE))
			{
				OpenPopup("##add_script");
			}
			if (BeginPopup("##add_script"))
			{
				ResourceManager& rm = Engine::GetEngine()->GetResourceManager();
				std::vector<Resource *> scripts = rm.GetResourcesOfTypes_Alphabetical(ResourceType::SCRIPT);

				editor->GetSearchBars().script.Draw("Search", 100);

				// script_c will be nullptr if the object doesn't have one.
				ScriptComponent *script_c = nullptr;
				ComponentHandle<ScriptComponent> scriptHandle = object.GetComponent<ScriptComponent>();
				if (scriptHandle.IsValid())
				{
					script_c = scriptHandle.Get();
				}

				for (auto& script : scripts)
				{
					if (script_c == nullptr || !ObjectHasScript(script->Id(), script_c->scripts))
					{
						if (editor->GetSearchBars().script.PassFilter(script->FileName().c_str()))
						{
							if (Selectable(script->FileName().c_str()))
							{
								// Add the script if we need to.
								// TODO: Make this and adding the script one action.
								if (script_c == nullptr)
								{
									object.AddComponent<ScriptComponent>();
									Push_AddComponent(ScriptComponent);
									script_c = object.GetComponent<ScriptComponent>().Get();
								}

								// Add the script tself.
								script_c->scripts.emplace_back(LuaScript(script, object));
								editor->Push_Action({ 0, LuaScript(script, object), nullptr,{ object, true },
									[](EditorAction& a)
								{
									ComponentHandle<ScriptComponent> handle(a.handle);

									if (a.redo)
									{
										handle->scripts.emplace_back(a.current.GetData<LuaScript>());
									}
									else
									{
										ResourceID id = a.current.GetData<LuaScript>().GetResourceID();
										std::vector<LuaScript>& scripts = handle->scripts;

										for (size_t i = 0; i < scripts.size(); i++)
										{
											LuaScript& script = scripts[i];

											if (script.GetResourceID() == id)
											{
												scripts.erase(handle->scripts.begin() + i);
											}
										}
									}
								} });
							}
						}
					}
				}
				EndPopup();
			}
			Separator();
			if (Button("Sprite", COMPONENT_BUTTON_SIZE))
			{
				if (object.GetComponent<SpriteComponent>().IsValid())
				{
					HAS_COMPONENT;
				}
				else
				{
					object.AddComponent<SpriteComponent>();
					Push_AddComponent(SpriteComponent);
				}
			}
			if (Button("Particle System", COMPONENT_BUTTON_SIZE))
			{
				if (object.GetComponent<ParticleSystem>().IsValid())
				{
					HAS_COMPONENT;
				}
				else
				{
					object.AddComponent<ParticleSystem>();
					Push_AddComponent(ParticleSystem);
				}
			}
			if (Button("Camera", COMPONENT_BUTTON_SIZE))
			{
				if (object.GetComponent<Camera>().IsValid())
				{
					HAS_COMPONENT;
				}
				else
				{
					object.AddComponent<Camera>();
					Push_AddComponent(Camera);
				}
			}
			if (Button("Background", COMPONENT_BUTTON_SIZE))
			{
				if (object.GetComponent<BackgroundComponent>().IsValid())
				{
					HAS_COMPONENT;
				}
				else
				{
					object.AddComponent<BackgroundComponent>();
					Push_AddComponent(BackgroundComponent);
				}
			}

			EndPopup();
		}

		if (Button("Add Component##object"))
		{
			OpenPopup("Components");
		}

		SameLine();
		if (Button("Save Prefab##object"))
		{
			SavePrefab(object);
		}

		if (Checkbox("Saves With Level##object_savesWithLevel", &object.GetComponent<ObjectInfo>()->m_savesWithLevel))
		{
			editor->Push_Action({ !object.GetComponent<ObjectInfo>()->m_savesWithLevel, object.GetComponent<ObjectInfo>()->m_savesWithLevel, "savesWithLevel",{ object, true }, Action_General<ObjectInfo, bool> });
		}

		ImGui_ObjectInfo(object.GetComponent<ObjectInfo>().Get(), editor);


		// if object -> component
		// ImGui_Component(ComponetType *component);
		if (object.GetComponent<TransformComponent>().IsValid())
		{
			ImGui_Transform(object.GetComponent<TransformComponent>().Get(), object, editor);
		}

		// Check for RigidBody OR Static Collider, can only have one
		if (object.GetComponent<RigidBodyComponent>().IsValid())
		{
			ImGui_RigidBody(object.GetComponent<RigidBodyComponent>().Get(), object, editor);

			if (object.GetComponent<DynamicCollider2DComponent>().IsValid())
			{
				ImGui_Collider2D(&object.GetComponent<DynamicCollider2DComponent>().Get()->ColliderData(), object, editor);
			}
		}
		else if (object.GetComponent<StaticCollider2DComponent>().IsValid())
		{
			ImGui_Collider2D(&object.GetComponent<StaticCollider2DComponent>()->ColliderData(), object, editor);
		}
		else if (object.GetComponent<DynamicCollider2DComponent>().IsValid())
		{
			ImGui_Collider2D(&object.GetComponent<DynamicCollider2DComponent>().Get()->ColliderData(), object, editor);
		}

		if (object.GetComponent<ScriptComponent>().IsValid())
		{
			ImGui_Script(object.GetComponent<ScriptComponent>().Get(), object, editor);
		}

		if (object.GetComponent<SpriteComponent>().IsValid())
		{
			ImGui_Sprite(object.GetComponent<SpriteComponent>().Get(), object, editor);
		}

		if (object.GetComponent<ParticleSystem>().IsValid())
		{
			ImGui_Particles(object.GetComponent<ParticleSystem>().Get(), object, editor);
		}

		if (object.GetComponent<Camera>().IsValid())
		{
			ImGui_Camera(object.GetComponent<Camera>().Get(), object, editor);
		}

		if (object.GetComponent<BackgroundComponent>().IsValid())
		{
			ImGui_Background(object.GetComponent<BackgroundComponent>().Get(), object, editor);
		}

		End();
	}
	else
	{
		// No Object was selected so display a default

		const char *name = "GameObject - No Object Selected###GAMEOBJECT_ID";

		ImVec2 windowSize = GAMEOBJECT_WINDOW_SIZE;
		windowSize.x *= Editor::GetUiScale();
		windowSize.y *= Editor::GetUiScale();
		SetNextWindowSize(windowSize);
		SetNextWindowPos(GAMEOBJECT_WINDOW_POS, ImGuiCond_Once);
		Begin(name, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);
		End();
	}
}


void ImGui_GameObject_Multi(std::vector<std::pair<GameObject, glm::vec2>>& objects, Editor *editor)
{
#if 1
	for (size_t i = 0; i < objects.size(); i++)
	{
		GameObject object = objects[i].first;
		
		TransformComponent *transform = object.GetComponent<TransformComponent>().Get();
		DebugGraphic::DrawSquare(transform->GetPosition(), glm::vec2(transform->GetScale()) + glm::vec2(0.025f, 0.025f), (transform->GetRotation() * 3.14159265f) / 180, glm::vec4(0, 1, 1, 1));
	}

#else
	(void)objects;
	(void)editor;
#endif
}


//
// Component ImGui stuff
// ----------------------
// Binds the imgui function calls to the ObjectInfo Component
void ImGui_ObjectInfo(ObjectInfo *info, Editor *editor)
{
	if (info)
	{
		Separator();

		EditorComponentHandle handle = { info->m_id, true };

		// Display the ID and make the name clickable for editing
		Text("ID: %d | ", info->m_id & 0xFFFFFF);

		SameLine();
		if (Selectable(info->m_name.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(CalcTextSize(info->m_name.c_str()).x, 0)))
		{
			OpenPopup("Edit Name###name_popup");
		}

		if (Button("Tags"))
		{
			// Check if we have tags
			if (info->m_tags.size())
			{
				OpenPopup("##object_info_tags");
			}
			else
			{
				editor->AddPopUp(PopUpWindow("This object has no tags.", 1.5f, PopUpPosition::Mouse));
			}
		}

		if (BeginPopup("##object_info_tags"))
		{
			PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.44f, 0));
			for (auto& tag : info->m_tags)
			{
				if (Button("X##object_info_remove_tag", ImVec2(25, 0)))
				{
					editor->Push_Action({ static_cast<size_t>(hash(tag.second.c_str())), tag.second, nullptr, handle, Action_General_TagsRemove });
					info->m_tags.erase(tag.first);
					break;
				}
				SameLine();
				Text(tag.second.c_str());
			}
			PopStyleVar();

			EndPopup();
		}

		SameLine();

		if (Button("Add Tag"))
		{
			OpenPopup("##object_info_tags_add");
		}

		if (BeginPopup("##object_info_tags_add"))
		{
			char buffer[128] = { 0 };
			if (InputText("Tag", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
			{
				info->AddTag(buffer);
				editor->AddPopUp(PopUpWindow("Tag Added.", 1.0f, PopUpPosition::Mouse));

				std::string name_save = buffer;

				editor->Push_Action({ static_cast<size_t>(hash(buffer)), name_save, nullptr, handle, Action_General_TagsAdd });

				CloseCurrentPopup();
			}

			EndPopup();
		}
	}
}

#define SLIDER_STEP 0.01f
#define ID_MASK 0xFFFFFF


static void Display_Hierarchy(GameObject object)
{
	// Get the children
	auto children = object.GetComponent<HierarchyComponent>();

	// Foreach child check if they have more kids and display those
	for (auto& child : children->GetList())
	{
		// Has Kids
		if (child.GetComponent<HierarchyComponent>().IsValid())
		{
			// Add a space for spacing
			std::string name = child.GetName();
			name += " ";

			// Show a tree node
			if (TreeNode(name.c_str()))
			{
				// Call this function for the child
				Display_Hierarchy(child);

				Separator();
				TreePop();
			}
		}
		else
		{
			// Just display the GameObject
			Text(child.GetName().c_str());
		}
	}
}


// Binds the imgui function calls to the Transform Component
void ImGui_Transform(TransformComponent *transform, GameObject object, Editor *editor)
{
	// Draws the Selection Box
	glm::vec2 scale(transform->GetScale());
	DebugGraphic::DrawSquare(transform->GetPosition(), scale + glm::vec2(0.025f, 0.025f), (transform->GetRotation() * 3.14159265f) / 180, glm::vec4(0,1,0,1));

	EditorComponentHandle handle = { object.Getid(), true };
	
	SameLine();
	if (Button("Children"))
	{
		OpenPopup("Children##transform_parent");
	}

	if (BeginPopup("Children##transform_parent"))
	{
		if (object.GetComponent<HierarchyComponent>().IsValid())
		{
			Display_Hierarchy(object);
		}
		else
		{
			editor->AddPopUp(PopUpWindow("Has no children.", 2.0f, PopUpPosition::Mouse));
			CloseCurrentPopup();
		}
		EndPopup();
	}

	SameLine();
	if (transform->GetParent())
	{
		if (Button("Remove Parent##remove_parent_button"))
		{
			transform->m_parent.GetComponent<HierarchyComponent>()->Remove(object);
			transform->SetParent(0);
		}
		else
		{
			SameLine();
			Text("%d | %s", transform->GetParent().Getid() & ID_MASK, transform->GetParent().GetComponent<ObjectInfo>()->m_name.c_str());
		}
	}
	else
	{
		if (Button("Add Parent"))
		{
			OpenPopup("Add Parent##add_parent_popup");
		}

		if (BeginPopup("Add Parent##add_parent_popup"))
		{
			GameObject parent = transform->GetParent();
			if (Choose_Parent_ObjectList(editor, transform, object))
			{
				editor->Push_Action({ parent, transform->m_parent, "parent", handle, Action_General<TransformComponent, decltype(parent)> });

				// Turn off Gravity
				if (object.GetComponent<RigidBodyComponent>().IsValid())
				{
					object.GetComponent<RigidBodyComponent>()->SetGravity(glm::vec3());
				}
			}
			EndPopup();
		}
	}

	Separator();
	ImGui::Text("Position");
	if (transform->GetParent())
	{
		Text("X: %f", transform->GetPosition().x);
		Text("Y: %f", transform->GetPosition().y);

		// Position Widgets
		Drag_Vec("X Offset##transform_position", transformSave.m_localPosition, transform->m_localPosition.x, transform->m_localPosition);
		Drag_Vec("Y Offset##transform_position", transformSave.m_localPosition, transform->m_localPosition.y, transform->m_localPosition);

		DragRelease_Type_CastAll(TransformComponent, transformSave.m_localPosition, transform->m_localPosition, "position", glm::vec2);
	}
	else
	{
		Drag_Vec("X##transform_position", transformSave.m_localPosition, transform->m_localPosition.x, transform->m_localPosition);
		Drag_Vec("Y##transform_position", transformSave.m_localPosition, transform->m_localPosition.y, transform->m_localPosition);

		DragRelease_Type_CastAll(TransformComponent, transformSave.m_localPosition, transform->m_localPosition, "position", glm::vec2);
	}

	Separator();
	ImGui::Text("Scale");

	ComponentHandle<SpriteComponent> sprite = object.GetComponent<SpriteComponent>();
	if (sprite.IsValid())
	{
		// Match the gnome by default.
		static const float DEFAULT_PIXELS_PER_UNIT = 256;
		static float pixelsPerUnit = DEFAULT_PIXELS_PER_UNIT;
		if (ImGui::Button("Scale to Sprite"))
		{
			pixelsPerUnit = DEFAULT_PIXELS_PER_UNIT;
			ImGui::OpenPopup("##scaleToSpritePopup");
		}
		if (ImGui::BeginPopup("##scaleToSpritePopup"))
		{
			ImGui::InputFloat("Pixels per Unit", &pixelsPerUnit);
			ImGui::SameLine();
			if (ImGui::Button("SCALE##pixelScaleConfirm"))
			{
				Resource *spriteResource = engine->GetResourceManager().Get(sprite->GetResourceID());
				Texture *text = reinterpret_cast<Texture *>(spriteResource->Data());

				glm::vec3 newScale(text->PixelWidth() / pixelsPerUnit, text->PixelHeight() / pixelsPerUnit, 1);
				transformSave.m_scale = transform->m_scale;
				editor->Push_Action({transform->m_scale, newScale,  "scale", handle, Action_General<TransformComponent, glm::vec3> });
				transform->m_scale = newScale;

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	Drag_Vec("X##scale", transformSave.m_scale, transform->m_scale.x, transform->m_scale);
	Drag_Vec("Y##scale", transformSave.m_scale, transform->m_scale.y, transform->m_scale);

	DragRelease(TransformComponent, transformSave.m_scale, transform->m_scale, "scale");
		
	if (ObjectsMatchScale[object] == true)
	{
		if (object.GetComponent<StaticCollider2DComponent>().Get())
		{
			object.GetComponent<StaticCollider2DComponent>()->ColliderData().SetDimensions(transform->m_scale);
		}
		else if (object.GetComponent<DynamicCollider2DComponent>().Get())
		{
			object.GetComponent<DynamicCollider2DComponent>()->ColliderData().SetDimensions(transform->m_scale);
		}
		Separator();
	}

	Separator();
	int z_layer = static_cast<int>(transform->GetZLayer());
	if (InputInt("Z-Layer##transform", &z_layer, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		editor->Push_Action({ transform->m_localPosition.z, static_cast<float>(z_layer), "zLayer", handle, Action_General<TransformComponent, float> });
		transform->SetZLayer(static_cast<float>(z_layer));
	}

	Drag_Float_Speed("Rotation##transform", transformSave.m_rotation, transform->m_rotation, 1.0f);
	DragRelease(TransformComponent, transformSave.m_rotation, transform->m_rotation, "rotation");
}


// Binds the imgui function calls to the RigidBody Component
void ImGui_RigidBody(RigidBodyComponent *rb, GameObject object, Editor * editor)
{
	if (CollapsingHeader("RigidBody"))
	{
		EditorComponentHandle handle = { object.Getid(), true };

		if (Button("Remove##rigidbody"))
		{
			//bool hasDynCollider = false;
			if (object.GetComponent<DynamicCollider2DComponent>().IsValid())
			{
				object.DeleteComponent<DynamicCollider2DComponent>();
				//hasDynCollider = true;
				editor->AddPopUp(PopUpWindow("Removed Dynamic Collider.", 1.5f, PopUpPosition::Mouse));
			}

			editor->Push_Action({ *rb, 0, nullptr, handle, Action_DeleteComponent<RigidBodyComponent> });
			object.DeleteComponent<RigidBodyComponent>();
			return;
		}

		if (TreeNode("Acceleration"))
		{			
			Drag_Vec("X##acceleration", rigidBodySave.m_acceleration, rb->m_acceleration.x, rb->m_acceleration);
			Drag_Vec("Y##acceleration", rigidBodySave.m_acceleration, rb->m_acceleration.y, rb->m_acceleration);

			DragRelease(RigidBodyComponent, rigidBodySave.m_acceleration, rigidBodySave.m_acceleration, "acceleration");
			
			TreePop();
			Separator();
		}
		if (TreeNode("Velocity"))
		{
			Drag_Vec("X##rigidbody_velocity", rigidBodySave.m_velocity, rb->m_velocity.x, rb->m_velocity);
			Drag_Vec("Y##rigidbody_velocity", rigidBodySave.m_velocity, rb->m_velocity.y, rb->m_velocity);

			DragRelease(RigidBodyComponent, rigidBodySave.m_velocity, rb->m_velocity, "velocity");

			TreePop();
			Separator();
		}
		if (TreeNode("Gravity"))
		{
			Drag_Vec("X##gravity", rigidBodySave.m_gravity, rb->m_gravity.x, rb->m_gravity);
			Drag_Vec("Y##gravity", rigidBodySave.m_gravity, rb->m_gravity.y, rb->m_gravity);

			DragRelease(RigidBodyComponent, rigidBodySave.m_gravity, rb->m_gravity, "gravity");
			
			TreePop();
			Separator();
		}

		float mass = 1 / rb->Mass();
		Drag("Mass##rigidBody", rigidBodySave.m_inverseMass, mass);

		if (Input::IsReleased(Drag_Key) && widget_click["rigidBodySave.m_inverseMass"] == true)
		{
			editor->Push_Action({ rb->Mass(), 1 / mass, "mass", handle, Action_General<RigidBodyComponent, float> });
			widget_click["rigidBodySave.m_inverseMass"] = false;
		}

		rb->SetMass(1 / mass);
	}
}


// Binds the imgui function calls to the Collider Component
void ImGui_Collider2D(Collider2D *collider, GameObject object, Editor * editor)
{
	if (CollapsingHeader("Collider"))
	{
		EditorComponentHandle handle = { object.Getid(), true };
		if (Button("Remove##collider"))
		{
			if (collider->isStatic())
			{
				editor->Push_Action({ *collider, 0, nullptr, handle, Action_DeleteComponent<StaticCollider2DComponent> });
				object.DeleteComponent<StaticCollider2DComponent>();
			}
			else
			{
				editor->Push_Action({ *collider, 0, nullptr, handle, Action_DeleteComponent<DynamicCollider2DComponent> });
				object.DeleteComponent<DynamicCollider2DComponent>();
			}
			return;
		}

		bool dynamic = collider->isStatic() ? false : true;

		if (Checkbox("Dynamic Collider", &dynamic))
		{
			if (dynamic)
			{
				object.AddComponent<RigidBodyComponent>();
				object.AddComponent<DynamicCollider2DComponent>(object.GetComponent<StaticCollider2DComponent>().Get());
				object.DeleteComponent<StaticCollider2DComponent>();
				return;
			}
			else
			{
				object.AddComponent<StaticCollider2DComponent>(object.GetComponent<DynamicCollider2DComponent>().Get());
				object.DeleteComponent<DynamicCollider2DComponent>();
				object.DeleteComponent<RigidBodyComponent>();
				return;
			}
		}
		SameLine();
		if (Checkbox("Match Scale##collider", &(ObjectsMatchScale[object].value)))
		{
			colliderSave.m_dimensions = collider->m_dimensions;

			if (collider->isStatic())
			{
				editor->Push_Action({ colliderSave.m_dimensions, collider->m_dimensions, "dimensions",
					handle, Action_General_Collider<StaticCollider2DComponent, bool> });
			}
			else
			{
				editor->Push_Action({ colliderSave.m_dimensions, collider->m_dimensions, "dimensions",
					handle, Action_General_Collider<DynamicCollider2DComponent, bool> });
			}

			if (ObjectsMatchScale[object].value)
			{
				collider->m_dimensions = object.GetComponent<TransformComponent>()->GetScale();
			}
		}

		if (TreeNode("Dimensions"))
		{
			if (ObjectsMatchScale[object])
			{
				collider->m_dimensions = object.GetComponent<TransformComponent>()->GetScale();

				//if (collider->isStatic())
				//{
				//	editor->Push_Action({ colliderSave.m_dimensions, collider->m_dimensions, "dimensions",
				//		handle, Action_General_Collider<StaticCollider2DComponent> });
				//}
				//else
				//{
				//	editor->Push_Action({ colliderSave.m_dimensions, collider->m_dimensions, "dimensions",
				//		handle, Action_General_Collider<DynamicCollider2DComponent> });
				//}
			}

			Drag_Vec("X##collider_dim", colliderSave.m_dimensions, collider->m_dimensions.x, collider->m_dimensions);
			Drag_Vec("Y##collider_dim", colliderSave.m_dimensions, collider->m_dimensions.y, collider->m_dimensions);

			if (Input::IsReleased(Drag_Key))
			{						
				if (widget_click["colliderSave.m_dimensions"] == true)
				{
					if (collider->isStatic())
					{
						editor->Push_Action({ colliderSave.m_dimensions, collider->m_dimensions, "dimensions",
							handle, Action_General_Collider<StaticCollider2DComponent, decltype(collider->m_dimensions)> });
					}
					else
					{
						editor->Push_Action({ colliderSave.m_dimensions, collider->m_dimensions, "dimensions",
							handle, Action_General_Collider<DynamicCollider2DComponent, decltype(collider->m_dimensions)> });
					}
					widget_click["colliderSave.m_dimensions"] = false;
				}
			}

			TreePop();
			Separator();
		}
		if (TreeNode("Offset"))
		{
			Drag_Vec("X##collider_offset", colliderSave.m_offset, collider->m_offset.x, collider->m_offset);
			Drag_Vec("Y##collider_offset", colliderSave.m_offset, collider->m_offset.y, collider->m_offset);

			if (Input::IsReleased(Drag_Key))
			{
				// Check if we need to save the action for static or dynamic
				if (widget_click["colliderSave.m_offset"] == true)
				{
					// Check if we need to save the action for static or dynamic
					if (collider->isStatic())
					{
						editor->Push_Action({ colliderSave.m_offset, collider->m_offset, "offset", handle, Action_General_Collider<StaticCollider2DComponent, decltype(collider->m_offset)> });
					}
					else
					{
						editor->Push_Action({ colliderSave.m_offset, collider->m_offset, "offset", handle, Action_General_Collider<DynamicCollider2DComponent, decltype(collider->m_offset)> });
					}
					widget_click["colliderSave.m_offset"] = false;
				}
			}

			Drag("Rotation##collider_offset", colliderSave.m_rotationOffset, collider->m_rotationOffset);
			if (Input::IsReleased(Drag_Key))
			{
				// Check if we need to save the action for static or dynamic
				if (widget_click["colliderSave.m_rotationOffset"] == true)
				{
					// Check if we need to save the action for static or dynamic
					if (collider->isStatic())
					{
						editor->Push_Action({ colliderSave.m_rotationOffset, collider->m_rotationOffset, "rotationOffset", handle, Action_General_Collider<StaticCollider2DComponent, decltype(collider->m_rotationOffset)> });
					}
					else
					{
						editor->Push_Action({ colliderSave.m_rotationOffset, collider->m_rotationOffset, "rotationOffset", handle, Action_General_Collider<DynamicCollider2DComponent, decltype(collider->m_rotationOffset)> });
					}
					widget_click["colliderSave.m_rotationOffset"] = false;
				}
			}

			TreePop();
			Separator();
		}
		int index = collider->m_colliderShape;
		for (int i = 0; i < sizeof(Collider2D::colliderType) * 8; i++)
		{
			if (index & (1 << i))
			{
				if (i)
				{
					index = i - 1;
					break;
				}
			}
		}

		Drag_Float_Speed_MinMax("Elasticity##collider", colliderSave.m_selfElasticity, collider->m_selfElasticity, SLIDER_STEP, 0, 1);
		if (Input::IsReleased(Drag_Key) && widget_click["colliderSave.m_selfElasticity"] == true)
		{
			// Check if we need to save the action for static or dynamic
			if (collider->isStatic())
			{
				editor->Push_Action({ colliderSave.m_selfElasticity, collider->m_selfElasticity, "selfElasticity", handle, Action_General_Collider<StaticCollider2DComponent, decltype(collider->m_selfElasticity)> });
			}
			else
			{
				editor->Push_Action({ colliderSave.m_selfElasticity, collider->m_selfElasticity, "selfElasticity", handle, Action_General_Collider<DynamicCollider2DComponent, decltype(collider->m_selfElasticity)> });
			}
			widget_click["colliderSave.m_selfElasticity"] = false;
		}
		
		// Collision Type
		int type = static_cast<int>(collider->m_collisionType);
		if (Combo("Collision Type##collider", &type, ColliderTypeNames, static_cast<int>(Collider2D::collisionType::collision_types)))
		{
			if (collider->isStatic())
			{
				editor->Push_Action({ collider->m_collisionType, static_cast<Collider2D::collisionType>(type), "collisionType", handle, Action_General_Collider<StaticCollider2DComponent, decltype(collider->m_collisionType)> });
				collider->m_collisionType = static_cast<Collider2D::collisionType>(type);
			}
			else
			{
				editor->Push_Action({ collider->m_collisionType, static_cast<Collider2D::collisionType>(type), "collisionType", handle, Action_General_Collider<DynamicCollider2DComponent, decltype(collider->m_collisionType)> });
				collider->m_collisionType = static_cast<Collider2D::collisionType>(type);
			}
		}

		if (Combo("Collider Type##collider", &index, collider_types, static_cast<int>(Collider2D::colliderType::collider_max)))
		{

			int isStatic = static_cast<int>(collider->isStatic());

			switch (index)
			{
			case 0:
				if (collider->m_colliderShape != (Collider2D::colliderType::colliderBox | isStatic))
				{
					if (collider->isStatic())
					{
						editor->Push_Action({ collider->m_colliderShape, isStatic | Collider2D::colliderType::colliderBox, "colliderShape", handle, Action_General_Collider<StaticCollider2DComponent, decltype(collider->m_colliderShape)> });
					}
					else
					{
						editor->Push_Action({ collider->m_colliderShape, isStatic | Collider2D::colliderType::colliderBox, "colliderShape", handle, Action_General_Collider<DynamicCollider2DComponent, decltype(collider->m_colliderShape)> });
					}
					collider->m_colliderShape = isStatic | Collider2D::colliderType::colliderBox;
				}
				break;

			case 1:
				if (collider->m_colliderShape != (isStatic | Collider2D::colliderType::colliderCircle))
				{
					if (collider->isStatic())
					{
						editor->Push_Action({ collider->m_colliderShape, isStatic | Collider2D::colliderType::colliderCircle, "colliderShape", handle, Action_General_Collider<StaticCollider2DComponent, decltype(collider->m_colliderShape)> });
					}
					else
					{
						editor->Push_Action({ collider->m_colliderShape, isStatic | Collider2D::colliderType::colliderCircle, "colliderShape", handle, Action_General_Collider<DynamicCollider2DComponent, decltype(collider->m_colliderShape)> });
					}
					collider->m_colliderShape = isStatic | Collider2D::colliderType::colliderCircle;
				}
				break;

			case 2:
				if (collider->m_colliderShape != (isStatic | Collider2D::colliderType::colliderCapsule))
				{
					if (collider->isStatic())
					{
						editor->Push_Action({ collider->m_colliderShape, isStatic | Collider2D::colliderType::colliderCapsule, "colliderShape", handle, Action_General_Collider<StaticCollider2DComponent, decltype(collider->m_colliderShape)> });
					}
					else
					{
						editor->Push_Action({ collider->m_colliderShape, isStatic | Collider2D::colliderType::colliderCapsule, "colliderShape", handle, Action_General_Collider<DynamicCollider2DComponent, decltype(collider->m_colliderShape)> });
					}
					collider->m_colliderShape = isStatic | Collider2D::colliderType::colliderCapsule;
				}
				break;

			default:
				break;
			};
		}
		Separator();
		
		// Collision Layers
		if (TreeNode("Collision Layers"))
		{
			int layer = collider->m_collisionLayer;

			Columns(2, nullptr, false);
			if (RadioButton("All Collision", &layer, collisionLayers::allCollision))
			{
				if (collider->isStatic())
				{
					editor->Push_Action({ collider->m_collisionLayer, CollisionLayer(layer), "collisionLayer",
						handle, Action_General_Collider<StaticCollider2DComponent, decltype(collider->m_collisionLayer)> });
				}
				else
				{
					editor->Push_Action({ collider->m_collisionLayer, CollisionLayer(layer), "collisionLayer",
						handle, Action_General_Collider<DynamicCollider2DComponent, decltype(collider->m_collisionLayer)> });
				}

				collider->m_collisionLayer = CollisionLayer(layer);
			}
			
			NextColumn();
			
			if (RadioButton("No Collision", &layer, collisionLayers::noCollision))
			{
				if (collider->isStatic())
				{
					editor->Push_Action({ collider->m_collisionLayer, CollisionLayer(layer), "collisionLayer",
						handle, Action_General_Collider<StaticCollider2DComponent, decltype(collider->m_collisionLayer)> });
				}
				else
				{
					editor->Push_Action({ collider->m_collisionLayer, CollisionLayer(layer), "collisionLayer",
						handle, Action_General_Collider<DynamicCollider2DComponent, decltype(collider->m_collisionLayer)> });
				}

				collider->m_collisionLayer = CollisionLayer(layer);
			}
			
			NextColumn();
			
			if (RadioButton("Player", &layer, collisionLayers::player))
			{
				if (collider->isStatic())
				{
					editor->Push_Action({ collider->m_collisionLayer, CollisionLayer(layer), "collisionLayer",
						handle, Action_General_Collider<StaticCollider2DComponent, decltype(collider->m_collisionLayer)> });
				}
				else
				{
					editor->Push_Action({ collider->m_collisionLayer, CollisionLayer(layer), "collisionLayer",
						handle, Action_General_Collider<DynamicCollider2DComponent, decltype(collider->m_collisionLayer)> });
				}

				collider->m_collisionLayer = CollisionLayer(layer);
			}
			
			NextColumn();
			
			if (RadioButton("Decor", &layer, collisionLayers::decor))
			{
				if (collider->isStatic())
				{
					editor->Push_Action({ collider->m_collisionLayer, CollisionLayer(layer), "collisionLayer",
						handle, Action_General_Collider<StaticCollider2DComponent, decltype(collider->m_collisionLayer)> });
				}
				else
				{
					editor->Push_Action({ collider->m_collisionLayer, CollisionLayer(layer), "collisionLayer",
						handle, Action_General_Collider<DynamicCollider2DComponent, decltype(collider->m_collisionLayer)> });
				}

				collider->m_collisionLayer = CollisionLayer(layer);
			}
			
			NextColumn();
			
			if (RadioButton("Ground", &layer, collisionLayers::ground))
			{
				if (collider->isStatic())
				{
					editor->Push_Action({ collider->m_collisionLayer, CollisionLayer(layer), "collisionLayer",
						handle, Action_General_Collider<StaticCollider2DComponent, decltype(collider->m_collisionLayer)> });
				}
				else
				{
					editor->Push_Action({ collider->m_collisionLayer, CollisionLayer(layer), "collisionLayer",
						handle, Action_General_Collider<DynamicCollider2DComponent, decltype(collider->m_collisionLayer)> });
				}

				collider->m_collisionLayer = CollisionLayer(layer);
			}
			
			NextColumn();
			
			if (RadioButton("Ally Projectile", &layer, collisionLayers::allyProjectile))
			{
				if (collider->isStatic())
				{
					editor->Push_Action({ collider->m_collisionLayer, CollisionLayer(layer), "collisionLayer",
						handle, Action_General_Collider<StaticCollider2DComponent, decltype(collider->m_collisionLayer)> });
				}
				else
				{
					editor->Push_Action({ collider->m_collisionLayer, CollisionLayer(layer), "collisionLayer",
						handle, Action_General_Collider<DynamicCollider2DComponent, decltype(collider->m_collisionLayer)> });
				}

				collider->m_collisionLayer = CollisionLayer(layer);
			}
			
			NextColumn();
			
			if (RadioButton("Enemy", &layer, collisionLayers::enemy))
			{
				if (collider->isStatic())
				{
					editor->Push_Action({ collider->m_collisionLayer, CollisionLayer(layer), "collisionLayer",
						handle, Action_General_Collider<StaticCollider2DComponent, decltype(collider->m_collisionLayer)> });
				}
				else
				{
					editor->Push_Action({ collider->m_collisionLayer, CollisionLayer(layer), "collisionLayer",
						handle, Action_General_Collider<DynamicCollider2DComponent, decltype(collider->m_collisionLayer)> });
				}

				collider->m_collisionLayer = CollisionLayer(layer);
			}
			
			NextColumn();

			if (RadioButton("Player 1", &layer, collisionLayers::player1))
			{
				if (collider->isStatic())
				{
					editor->Push_Action({ collider->m_collisionLayer, CollisionLayer(layer), "collisionLayer",
						handle, Action_General_Collider<StaticCollider2DComponent, decltype(collider->m_collisionLayer)> });
				}
				else
				{
					editor->Push_Action({ collider->m_collisionLayer, CollisionLayer(layer), "collisionLayer",
						handle, Action_General_Collider<DynamicCollider2DComponent, decltype(collider->m_collisionLayer)> });
				}

				collider->m_collisionLayer = CollisionLayer(layer);
			}

			NextColumn();

			if (RadioButton("Player 2", &layer, collisionLayers::player2))
			{
				if (collider->isStatic())
				{
					editor->Push_Action({ collider->m_collisionLayer, CollisionLayer(layer), "collisionLayer",
						handle, Action_General_Collider<StaticCollider2DComponent, decltype(collider->m_collisionLayer)> });
				}
				else
				{
					editor->Push_Action({ collider->m_collisionLayer, CollisionLayer(layer), "collisionLayer",
						handle, Action_General_Collider<DynamicCollider2DComponent, decltype(collider->m_collisionLayer)> });
				}

				collider->m_collisionLayer = CollisionLayer(layer);
			}

			NextColumn();

			if (RadioButton("Player 3", &layer, collisionLayers::player3))
			{
				if (collider->isStatic())
				{
					editor->Push_Action({ collider->m_collisionLayer, CollisionLayer(layer), "collisionLayer",
						handle, Action_General_Collider<StaticCollider2DComponent, decltype(collider->m_collisionLayer)> });
				}
				else
				{
					editor->Push_Action({ collider->m_collisionLayer, CollisionLayer(layer), "collisionLayer",
						handle, Action_General_Collider<DynamicCollider2DComponent, decltype(collider->m_collisionLayer)> });
				}

				collider->m_collisionLayer = CollisionLayer(layer);
			}

			NextColumn();

			if (RadioButton("Player 4", &layer, collisionLayers::player4))
			{
				if (collider->isStatic())
				{
					editor->Push_Action({ collider->m_collisionLayer, CollisionLayer(layer), "collisionLayer",
						handle, Action_General_Collider<StaticCollider2DComponent, decltype(collider->m_collisionLayer)> });
				}
				else
				{
					editor->Push_Action({ collider->m_collisionLayer, CollisionLayer(layer), "collisionLayer",
						handle, Action_General_Collider<DynamicCollider2DComponent, decltype(collider->m_collisionLayer)> });
				}

				collider->m_collisionLayer = CollisionLayer(layer);
			}

			Columns();

			// Check if we need to save the action for static or dynamic

			TreePop();
		}
	}
}


// Binds the imgui function calls to the Script Component
void ImGui_Script(ScriptComponent *script_c, GameObject object, Editor * editor)
{
	// The script component is a hidden container for individual scripts. Don't show it.

	int scriptCount = script_c->scripts.size();
	for (int i = 0; i < scriptCount; i++)
	{
		bool scriptDeleted = !ImGui_IndividualScript(script_c->scripts[i], script_c, object, editor);

		if (scriptDeleted)
		{
			// If we remove the last script, remove the script component.
			if (scriptCount == 1)
			{
				EditorComponentHandle handle = { object.Getid(), true };
				editor->Push_Action({ *script_c, 0, nullptr, handle, Action_DeleteComponent<ScriptComponent> });
				object.DeleteComponent<ScriptComponent>();
				return;
			}

			// Update counts and position so we loop properly.
			--scriptCount;
			--i;
		}
	}
}


// Binds the imgui function calls to the Sprite Component
void ImGui_Sprite(SpriteComponent *sprite, GameObject object, Editor * editor)
{
	if (CollapsingHeader("Sprite", ImGuiTreeNodeFlags_Framed))
	{
		EditorComponentHandle handle = { object.Getid(), true };
		TextureHandler& texture = sprite->m_TextureHandler;

		if (Button("Remove##sprite"))
		{
			editor->Push_Action({ *sprite, 0, nullptr, handle, Action_DeleteComponent<SpriteComponent> });
			object.DeleteComponent<SpriteComponent>();
			return;
		}

		SameLine();
		if (Button("Colors##sprite_color"))
		{
			if (!widget_click["Colors##sprite_color"])
			{
				spriteSave.m_Color = sprite->m_Color;
				widget_click["Colors##sprite_color"] = true;
			}

			OpenPopup("##sprite_color_picker");
		}

		SameLine();
		if (Button("Reset##sprite_sprite_reset"))
		{
			ResourceID temp = texture.m_Texture->Id();
			sprite->SetTextureID(-1);
			editor->Push_Action({ temp, texture.m_Texture->Id(), "resourceID", handle, Action_General<SpriteComponent, ResourceID> });
		}

		if (BeginPopup("##sprite_color_picker"))
		{
			ColorPicker4("Sprite Color", &sprite->m_Color.x, ImGuiColorEditFlags_AlphaBar);

			EndPopup();
		}
		else if (widget_click["Colors##sprite_color"] == true)
		{
			editor->Push_Action({ spriteSave.m_Color, sprite->m_Color, "color", handle, Action_General<SpriteComponent, glm::vec4> });
			widget_click["Colors##sprite_color"] = false;
		}


		if (Checkbox("Tiled", &sprite->m_TextureHandler.m_IsTiling))
		{
			editor->Push_Action({ !sprite->m_TextureHandler.m_IsTiling, sprite->m_TextureHandler.m_IsTiling, "isTiling", handle,
			[](EditorAction& a)
			{
				ComponentHandle<SpriteComponent> handle(a.handle);
				meta::Any texture(handle->GetTextureHandlerPointer());

				if (handle.GetGameObject().IsValid())
				{
					if (a.redo)
					{
						texture.SetPointerMember(a.name, a.current.GetData<decltype(sprite->m_TextureHandler.m_IsTiling)>());
					}
					else
					{
						texture.SetPointerMember(a.name, a.save.GetData<decltype(sprite->m_TextureHandler.m_IsTiling)>());
					}
				}
			}
			});
		}

		if (TreeNode("Tiled Amount##sprite_titled"))
		{
			Drag_Vec_MinMax("X##sprite_titled_amount", spriteSave.m_TileAmount, sprite->m_TextureHandler.m_TileAmount.x, sprite->m_TextureHandler.m_TileAmount, 0, FLT_MAX);
			Drag_Vec_MinMax("Y##sprite_titled_amount", spriteSave.m_TileAmount, sprite->m_TextureHandler.m_TileAmount.y, sprite->m_TextureHandler.m_TileAmount, 0, FLT_MAX);

			if (Input::IsReleased(Drag_Key) && widget_click["spriteSave.m_TileAmount"] == true)
			{
				editor->Push_Action({ spriteSave.m_TileAmount, sprite->m_TextureHandler.m_TileAmount, "tileAmount", handle,
					[](EditorAction& a)
				{
					ComponentHandle<SpriteComponent> handle(a.handle);
					meta::Any texture(handle->GetTextureHandlerPointer());

					if (handle.GetGameObject().IsValid())
					{
						if (a.redo)
						{
							texture.SetPointerMember(a.name, a.current.GetData<decltype(sprite->m_TextureHandler.m_TileAmount)>());
						}
						else
						{
							texture.SetPointerMember(a.name, a.save.GetData<decltype(sprite->m_TextureHandler.m_TileAmount)>());
						}
					}
				}
				});
				widget_click["spriteSave.m_TileAmount"] = true;
			}

			TreePop();
		}

		if (texture.m_IsAnimated)
		{
			Drag_Float_Speed_MinMax("Frame Rate##sprites", spriteSave.AT_fps, texture.m_FPS, 0.05f, 0, FLT_MAX);
			DragRelease(SpriteComponent, spriteSave.AT_fps, texture.m_FPS, "fps");

			int frame = texture.m_CurrentFrame;
			SliderInt("Frame", &frame, 0, reinterpret_cast<AnimatedTexture *>(texture.GetTexture())->GetMaxFrame() - 1);
			texture.m_CurrentFrame = frame;

			Drag_Float_Speed_MinMax("Frame Time##sprites", spriteSave.AT_timer, texture.m_Timer, 0.05f, 0, FLT_MAX);
			DragRelease(SpriteComponent, spriteSave.AT_timer, texture.m_Timer, "fps");
		}


		ResourceManager& rm = engine->GetResourceManager();
		std::vector<Resource *> sprites = rm.GetResourcesOfTypes_Alphabetical(ResourceType::TEXTURE, ResourceType::ANIMATION);

		ResourceID id = sprite->GetResourceID();

		Separator();
		editor->GetSearchBars().sprite.Draw("Search", -100.0f);

		BeginChild("Sprites", SPRITE_ASSETS_LIST_SIZE, true);
		for (auto resource : sprites)
		{
			if (resource->Id() == id)
			{
				PushStyleColor(ImGuiCol_Header, SPRITE_SELECTED_COLOR);
				Selectable(resource->FileName().c_str(), true);
				PopStyleColor();
				continue;
			}

			if (editor->GetSearchBars().sprite.PassFilter(resource->FileName().c_str()))
			{
				if (Selectable(resource->FileName().c_str()))
				{
					// Is resource ref counted, can I store pointers to them?
					sprite->SetTextureResource(resource);
					editor->Push_Action({ id, resource->Id(), "resourceID", handle, Action_General<SpriteComponent, ResourceID> });
				}
			}
		}
		EndChild();
	}
}


const char * const EmissionShape_Names[] =
{
	"Point",
	"Circle Volume",
	"Circle Edge",
	"Square Volume"
};


const char * const SimulationSpace_Names[] =
{
	"World",
	"Local"
};

// Binds the imgui function calls to the Particles Component
void ImGui_Particles(ParticleSystem *particles, GameObject object, Editor *editor)
{
	if (CollapsingHeader("Particle System"))
	{
		EditorComponentHandle handle = { object.Getid(), true };

		if (Button("Remove##particles"))
		{
			editor->Push_Action({ *particles, 0, nullptr, handle, Action_DeleteComponent<ParticleSystem> });
			object.DeleteComponent<ParticleSystem>();
			return;
		}

		SameLine();

		ParticleSettings& settings = particles->m_settings;

		Checkbox("Increase Max Particles", &settings.increasedMaxParticles);
		
		Checkbox("Looping", &settings.isLooping);

		Drag_Float_Speed_MinMax("Rate##particles", particleSave.emissionRate, settings.emissionRate, SLIDER_STEP, 0.005f, FLT_MAX);
		DragRelease_Func(ParticleSystem, particleSave.emissionRate, settings.emissionRate, "EmissionRate", Action_General_Particle<decltype(settings.emissionRate)>);

		Drag_Int_Speed_MinMax("Count##particles", particleSave.particlesPerEmission, settings.particlesPerEmission, 0.05f, 1, settings.increasedMaxParticles ? LARGE_MAX_PARTICLES : MAX_PARTICLES);
		DragRelease_Func(ParticleSystem, particleSave.particlesPerEmission, settings.particlesPerEmission, "ParticlesPerEmission", Action_General_Particle<decltype(settings.particlesPerEmission)>);

		Drag_Float_Speed("Emission Over Distance##particles", particleSave.emitOverDistanceAmount, settings.emitOverDistanceAmount, SLIDER_STEP);
		DragRelease_Func(ParticleSystem, particleSave.emitOverDistanceAmount, settings.emitOverDistanceAmount, "EmitOverDistanceAmount", Action_General_Particle<decltype(settings.emitOverDistanceAmount)>);

		if (TreeNode("Burst##particles"))
		{
			Checkbox("Emit At Start", &settings.emitBurstAtStart);
			Drag_Vec_MinMax("Repeat Time##particle", particleSave.burstEmission, settings.burstEmission.z, settings.burstEmission, 0, FLT_MAX);

			Drag_Vec_MinMax("Min Count##particle", particleSave.burstEmission, settings.burstEmission.x, settings.burstEmission, 0, settings.increasedMaxParticles ? LARGE_MAX_PARTICLES : MAX_PARTICLES);
			Drag_Vec_MinMax("Max Count##particle", particleSave.burstEmission, settings.burstEmission.y, settings.burstEmission, 0, settings.increasedMaxParticles ? LARGE_MAX_PARTICLES : MAX_PARTICLES);

			DragRelease_Func(ParticleSettings, particleSave.burstEmission, settings.burstEmission, "BurstEmission", Action_General_Particle<decltype(settings.burstEmission)>);
			Separator();
			TreePop();
		}

		

		Combo("Shape##particles", reinterpret_cast<int *>(&settings.emissionShape), EmissionShape_Names, _countof(EmissionShape_Names));

		Separator();

		if (TreeNode("Shape Scale##particles"))
		{
			Drag_Vec("X##particle_emission_rate", particleSave.emissionShapeScale, settings.emissionShapeScale.x, settings.emissionShapeScale);
			Drag_Vec("Y##particle_emission_rate", particleSave.emissionShapeScale, settings.emissionShapeScale.y, settings.emissionShapeScale);
			Drag_Vec_MinMax("Thiccness##particle_emission_rate", particleSave.emissionShapeScale, settings.emissionShapeScale.z, settings.emissionShapeScale, 0, 1);

			DragRelease_Func(ParticleSettings, particleSave.emissionShapeScale, settings.emissionShapeScale, "EmissionShapeScale", Action_General_Particle<decltype(settings.emissionShapeScale)>);

			TreePop();
		}

		Separator();

		Combo("Simulation Space##particles", reinterpret_cast<int *>(&settings.particleSpace), SimulationSpace_Names, _countof(SimulationSpace_Names));

		Drag("Emitter Lifetime##particles", particleSave.emitterLifetime, settings.emitterLifetime);
		DragRelease_Func(ParticleSettings, particleSave.emitterLifetime, settings.emitterLifetime, "EmitterLifetime", Action_General_Particle<decltype(settings.emitterLifetime)>);

		Drag("Lifetime##particles", particleSave.particleLifetime, settings.particleLifetime);
		DragRelease_Func(ParticleSettings, particleSave.particleLifetime, settings.particleLifetime, "EmitterLifetime", Action_General_Particle<decltype(settings.particleLifetime)>);

		//Drag("Lifetime Variance##particles", particleSave.particleLifetimeVariance, settings.particleLifetimeVariance);
		//DragRelease_Func(ParticleSettings, particleSave.particleLifetimeVariance, settings.particleLifetimeVariance, "ParticleLifetimeVariance", Action_General_Particle<decltype(settings.particleLifetimeVariance)>);

		if (TreeNode("Lifetime Variance##particles"))
		{
			Drag_Vec("Min##particle_life_var", particleSave.particleLifetimeVariance, settings.particleLifetimeVariance.x, settings.particleLifetimeVariance);
			Drag_Vec("Max##particle_life_var", particleSave.particleLifetimeVariance, settings.particleLifetimeVariance.y, settings.particleLifetimeVariance);

			DragRelease_Func(ParticleSettings, particleSave.particleLifetimeVariance, settings.particleLifetimeVariance, "ParticleLifetimeVariance", Action_General_Particle<decltype(settings.particleLifetimeVariance)>);

			TreePop();
		}


		Separator();

		if (TreeNode("Velocity##particles"))
		{
			Checkbox("Emit Away From Center", &settings.emitAwayFromCenter);

			Drag_Vec("X##particles_init_velocity", particleSave.startingVelocity, settings.startingVelocity.x, settings.startingVelocity);
			Drag_Vec("Y##particles_init_velocity", particleSave.startingVelocity, settings.startingVelocity.y, settings.startingVelocity);

			DragRelease_Func(ParticleSettings, particleSave.startingVelocity, settings.startingVelocity, "StartingVelocity", Action_General_Particle<decltype(settings.startingVelocity)>);

			if (TreeNode("Variance##particles"))
			{
				Drag_Vec("X Min##particles_variance_velocity", particleSave.startingVelocityVariance, settings.startingVelocityVariance.x, settings.startingVelocityVariance);
				Drag_Vec("Y Min##particles_variance_velocity", particleSave.startingVelocityVariance, settings.startingVelocityVariance.y, settings.startingVelocityVariance);
				Drag_Vec("X Max##particles_variance_velocity", particleSave.startingVelocityVariance, settings.startingVelocityVariance.z, settings.startingVelocityVariance);
				Drag_Vec("Y Max##particles_variance_velocity", particleSave.startingVelocityVariance, settings.startingVelocityVariance.w, settings.startingVelocityVariance);

				DragRelease_Func(ParticleSettings, particleSave.startingVelocityVariance, settings.startingVelocityVariance, "StartingVelocityVariance", Action_General_Particle<decltype(settings.startingVelocityVariance)>);

				TreePop();
			}

			Text("Speed Scale Over Life");
			Drag_Float_Speed("##particles_velocity_lifeScale", particleSave.velocityLimitAmount, settings.velocityLimitAmount,  SLIDER_STEP);
			DragRelease_Func(ParticleSettings, particleSave.velocityLimitAmount, settings.velocityLimitAmount, "VelocityLimitAmount", Action_General_Particle<decltype(settings.velocityLimitAmount)>);

			Separator();
			TreePop();
		}

		if (TreeNode("Acceleration##particles"))
		{
			Drag_Vec("X##particles_acceleration", particleSave.acceleration, settings.acceleration.x, settings.acceleration);
			Drag_Vec("Y##particles_acceleration", particleSave.acceleration, settings.acceleration.y, settings.acceleration);

			DragRelease_Func(ParticleSettings, particleSave.acceleration, settings.acceleration, "Acceleration", Action_General_Particle<decltype(settings.acceleration)>);

			Separator();
			TreePop();
		}

		if (TreeNode("Scale Progression##particles"))
		{
			Text("Start");
			Drag_Vec("X##particles_scale_start", particleSave.scaleOverTime, settings.scaleOverTime.x, settings.scaleOverTime);
			Drag_Vec("Y##particles_scale_start", particleSave.scaleOverTime, settings.scaleOverTime.y, settings.scaleOverTime);

			Text("End");
			Drag_Vec("X##particles_scale_end", particleSave.scaleOverTime, settings.scaleOverTime.z, particleSave.scaleOverTime);
			Drag_Vec("Y##particles_scale_end", particleSave.scaleOverTime, settings.scaleOverTime.w, particleSave.scaleOverTime);

			DragRelease_Func(ParticleSettings, particleSave.scaleOverTime, settings.scaleOverTime, "ScaleOverTime", Action_General_Particle<decltype(settings.scaleOverTime)>);

			Separator();
			TreePop();
		}

		if (TreeNode("Rotation##rotation_particles"))
		{
			Drag("Start##particles_rotation", particleSave.startRotation, settings.startRotation);
			DragRelease_Func(ParticleSystem, particleSave.startRotation, settings.startRotation, "StartRotation", Action_General_Particle<decltype(settings.scaleOverTime)>);


			//Drag("Variance##particles_rot_variance", particleSave.startRotationVariation, settings.startRotationVariation);
			//DragRelease_Func(ParticleSystem, particleSave.startRotationVariation, settings.startRotationVariation, "StartRotationVariation", Action_General_Particle<decltype(settings.startRotationVariation)>);
			if (TreeNode("Rotation Variance##particles"))
			{
				Drag_Vec("Min##particles_rot_variance", particleSave.startRotationVariation, settings.startRotationVariation.x, settings.startRotationVariation);
				Drag_Vec("Max##particles_rot_variance", particleSave.startRotationVariation, settings.startRotationVariation.y, settings.startRotationVariation);

				DragRelease_Func(ParticleSettings, particleSave.startRotationVariation, settings.startRotationVariation, "StartRotationVariation", Action_General_Particle<decltype(settings.startRotationVariation)>);

				TreePop();
			}


			Text("Rotation Over Life");
			Drag("##particles_rot_rate", particleSave.rotationRate, settings.rotationRate);
			DragRelease_Func(ParticleSystem, particleSave.rotationRate, settings.rotationRate, "RotationRate", Action_General_Particle<decltype(settings.rotationRate)>);

			Text("Speed Scaled Rotation");
			Drag("##particles_speedRot", particleSave.speedScaledRotation, settings.speedScaledRotation);
			DragRelease_Func(ParticleSystem, particleSave.speedScaledRotation, settings.speedScaledRotation, "SpeedScaledRotation", Action_General_Particle<decltype(settings.speedScaledRotation)>);

			Separator();
			TreePop();
		}

		if (TreeNode("Sprite Data##particles"))
		{
			if (TreeNode("Color##particles"))
			{
				Checkbox("Random Between 2 Colors", &settings.randomBetweenColors);

				if (Button("Start Color##particle_startColor"))
				{
					if (!widget_click["Start Color##particle_startColor"])
					{
						particleSave.startColor = settings.startColor;
						widget_click["Start Color##particle_startColor"] = true;
					}

					OpenPopup("##particle_startColor_picker");
				}

				if (BeginPopup("##particle_startColor_picker"))
				{
					ColorPicker4("Start Color", &settings.startColor.x, ImGuiColorEditFlags_AlphaBar);

					EndPopup();
				} 
				else if (widget_click["Colors##particle_startColor"] == true)
				{
					editor->Push_Action({ particleSave.startColor, settings.startColor, "color", handle, Action_General<SpriteComponent, glm::vec4> });
					widget_click["Colors##particle_startColor"] = false;
				}


				if (Button("End Color##particle_endColor"))
				{
					if (!widget_click["End Color##particle_endColor"])
					{
						particleSave.endColor = settings.endColor;
						widget_click["End Color##particle_endColor"] = true;
					}

					OpenPopup("##particle_endColor_picker");
				}

				if (BeginPopup("##particle_endColor_picker"))
				{
					ColorPicker4("End Color", &settings.endColor.x, ImGuiColorEditFlags_AlphaBar);

					EndPopup();
				}
				else if (widget_click["Colors##particle_endColor"] == true)
				{
					editor->Push_Action({ particleSave.endColor, settings.endColor, "color", handle, Action_General<SpriteComponent, glm::vec4> });
					widget_click["Colors##particle_endColor"] = false;
				}



				/*Text("Start Color");
				Drag_Vec_MinMax("R##particles_startColor", particleSave.startColor, settings.startColor.x, settings.startColor, 0, 1);
				Drag_Vec_MinMax("G##particles_startColor", particleSave.startColor, settings.startColor.y, settings.startColor, 0, 1);
				Drag_Vec_MinMax("B##particles_startColor", particleSave.startColor, settings.startColor.z, settings.startColor, 0, 1);
				Drag_Vec_MinMax("A##particles_startColor", particleSave.startColor, settings.startColor.w, settings.startColor, 0, 1);

				DragRelease_Func(ParticleSystem, particleSave.startColor, settings.startColor, "StartColor", Action_General_Particle<decltype(settings.startColor)>);

				Text("End Color");
				Drag_Vec_MinMax("R##particles_endColor", particleSave.endColor, settings.endColor.x, settings.endColor, 0, 1);
				Drag_Vec_MinMax("G##particles_endColor", particleSave.endColor, settings.endColor.y, settings.endColor, 0, 1);
				Drag_Vec_MinMax("B##particles_endColor", particleSave.endColor, settings.endColor.z, settings.endColor, 0, 1);
				Drag_Vec_MinMax("A##particles_endColor", particleSave.endColor, settings.endColor.w, settings.endColor, 0, 1);

				DragRelease_Func(ParticleSystem, particleSave.endColor, settings.endColor, "EndColor", Action_General_Particle<decltype(settings.endColor)>);
				*/

				Separator();
				TreePop();
			}

			if (TreeNode("Sprite"))
			{
				ResourceManager& rm = engine->GetResourceManager();
				std::vector<Resource *> sprites = rm.GetResourcesOfTypes_Alphabetical(ResourceType::TEXTURE, ResourceType::ANIMATION);

				
				if (Button("Reset##paritcles_sprite_reset"))
				{
					editor->Push_Action({ settings.texture_resourceID, -1, "TextureResourceID", handle, Action_General_Particle<ResourceID> });
					settings.texture_resourceID = -1;
				}
				SameLine();
				editor->GetSearchBars().particles.Draw("Search", 100.0f);

				BeginChild("Sprites", SPRITE_ASSETS_LIST_SIZE, true);
				for (auto resource : sprites)
				{
					if (resource->Id() == settings.texture_resourceID)
					{
						PushStyleColor(ImGuiCol_Header, SPRITE_SELECTED_COLOR);
						Selectable(resource->FileName().c_str(), true);
						PopStyleColor();
						continue;
					}

					if (editor->GetSearchBars().particles.PassFilter(resource->FileName().c_str()))
					{
						if (Selectable(resource->FileName().c_str()))
						{
							// Is resource ref counted, can I store pointers to them?
							editor->Push_Action({ settings.texture_resourceID, resource->Id(), "TextureResourceID", handle, Action_General_Particle<ResourceID> });
							settings.texture_resourceID = resource->Id();
						}
					}
				}
				EndChild();

				TreePop();
			}

			Separator();
			TreePop();
		}

		if (TreeNode("Trail##particles"))
		{
			if (Checkbox("Has Trail##particles", &settings.hasTrail))
			{
				editor->Push_Action({ !settings.hasTrail, settings.hasTrail, "HasTrail", handle, Action_General_Particle<bool> });
			}

			Drag("Rate##particles_trail", particleSave.trailEmissionRate, settings.trailEmissionRate);
			DragRelease_Func(ParticleSystem, particleSave.trailEmissionRate, settings.trailEmissionRate, "TrailEmissionRate", Action_General_Particle<decltype(settings.trailEmissionRate)>);

			Drag("Lifetime##particles_trail", particleSave.trailLifetime, settings.trailLifetime);
			DragRelease_Func(ParticleSystem, particleSave.trailLifetime, settings.trailLifetime, "TrailLifetime", Action_General_Particle<decltype(settings.trailLifetime)>);

			if (TreeNode("Color##trail_particles"))
			{

				if (Button("End Color##particle_trailColor"))
				{
					if (!widget_click["End Color##particle_trailColor"])
					{
						particleSave.trailEndColor = settings.trailEndColor;
						widget_click["End Color##particle_trailColor"] = true;
					}

					OpenPopup("##particle_trailColor_picker");
				}

				if (BeginPopup("##particle_trailColor_picker"))
				{
					ColorPicker4("End Color", &settings.trailEndColor.x, ImGuiColorEditFlags_AlphaBar);

					EndPopup();
				}
				else if (widget_click["Colors##particle_trailColor_picker"] == true)
				{
					editor->Push_Action({ particleSave.trailEndColor, settings.trailEndColor, "color", handle, Action_General<SpriteComponent, glm::vec4> });
					widget_click["Colors##particle_endColor"] = false;
				}

				/*
				Text("Start Color");
				Drag_Vec("R##particles_trail_startColor", particleSave.trailStartColor, settings.trailStartColor.x, settings.trailStartColor);
				Drag_Vec("G##particles_trail_startColor", particleSave.trailStartColor, settings.trailStartColor.y, settings.trailStartColor);
				Drag_Vec("B##particles_trail_startColor", particleSave.trailStartColor, settings.trailStartColor.z, settings.trailStartColor);
				Drag_Vec("A##particles_trail_startColor", particleSave.trailStartColor, settings.trailStartColor.w, settings.trailStartColor);
				
				DragRelease_Func(ParticleSystem, particleSave.trailStartColor, settings.trailStartColor, "TrailStartColor", Action_General_Particle<decltype(settings.trailStartColor)>);
				Separator();

				Text("End Color");
				Drag_Vec("R##particles_trail_endColor", particleSave.trailEndColor, settings.trailEndColor.x, settings.trailEndColor);
				Drag_Vec("G##particles_trail_endColor", particleSave.trailEndColor, settings.trailEndColor.y, settings.trailEndColor);
				Drag_Vec("B##particles_trail_endColor", particleSave.trailEndColor, settings.trailEndColor.z, settings.trailEndColor);
				Drag_Vec("A##particles_trail_endColor", particleSave.trailEndColor, settings.trailEndColor.w, settings.trailEndColor);

				DragRelease_Func(ParticleSystem, particleSave.trailEndColor, settings.trailEndColor, "TrailEndColor", Action_General_Particle<decltype(settings.trailEndColor)>);
				*/

				TreePop();
			}

			if (TreeNode("Sprite"))
			{
				ResourceManager& rm = engine->GetResourceManager();
				std::vector<Resource *> sprites = rm.GetResourcesOfTypes_Alphabetical(ResourceType::TEXTURE, ResourceType::ANIMATION);


				if (Button("Reset##paritcleTrail_sprite_reset"))
				{
					editor->Push_Action({ settings.trailTex_resourceID, -1, "TextureResourceID", handle, Action_General_Particle<ResourceID> });
					settings.trailTex_resourceID = -1;
				}
				SameLine();
				editor->GetSearchBars().particles.Draw("Search", 100.0f);

				BeginChild("Sprites", SPRITE_ASSETS_LIST_SIZE, true);
				for (auto resource : sprites)
				{
					if (resource->Id() == settings.trailTex_resourceID)
					{
						PushStyleColor(ImGuiCol_Header, SPRITE_SELECTED_COLOR);
						Selectable(resource->FileName().c_str(), true);
						PopStyleColor();
						continue;
					}

					if (editor->GetSearchBars().particles.PassFilter(resource->FileName().c_str()))
					{
						if (Selectable(resource->FileName().c_str()))
						{
							// Is resource ref counted, can I store pointers to them?
							editor->Push_Action({ settings.trailTex_resourceID, resource->Id(), "TextureResourceID", handle, Action_General_Particle<ResourceID> });
							settings.trailTex_resourceID = resource->Id();
						}
					}
				}
				EndChild();

				TreePop();
			}

			Separator();
			TreePop();
		}
	}
}


void ImGui_Camera(Camera *camera, GameObject object, Editor *editor)
{
	if (CollapsingHeader("Camera##camera_component"))
	{
		EditorComponentHandle handle = { object.Getid(), true };

		if (Button("Remove##camera"))
		{
			editor->Push_Action({ *camera, 0, nullptr, handle, Action_DeleteComponent<Camera> });
			object.DeleteComponent<Camera>();
			return;
		}

		SameLine();
		if (Button("Use##camera"))
		{
			editor->Push_Action({ Camera::GetActiveCamera(), camera, nullptr, handle, 
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
			camera->Use();
		}

		Drag_Float_Speed_MinMax("Zoom##camera", cameraSave.m_Zoom, camera->m_Zoom, SLIDER_STEP, 0, FLT_MAX);
		DragRelease(Camera, cameraSave.m_Zoom, camera->m_Zoom, "zoom");
	}
}


void ImGui_Background(BackgroundComponent *background, GameObject object, Editor *editor)
{
	if (CollapsingHeader("Background##background_component"))
	{
		EditorComponentHandle handle = { object.Getid(), true };

		if (Button("Remove##background_remove"))
		{
			editor->Push_Action({ *background, 0, nullptr, handle, Action_DeleteComponent<BackgroundComponent> });
			object.DeleteComponent<BackgroundComponent>();
			return;
		}


		int type = static_cast<int>(background->m_Type);
		if (RadioButton("Background##background_fg", &type, static_cast<int>(BACKGROUND_TYPE::BACKGROUND)))
		{
			editor->Push_Action({ static_cast<int>(background->m_Type), type, "backgroundType", handle, Action_General<BackgroundComponent, BACKGROUND_TYPE> });
			background->m_Type = static_cast<BACKGROUND_TYPE>(type);
		}
		SameLine();
		if (RadioButton("Parallax (Background)##background_bg", &type, static_cast<int>(BACKGROUND_TYPE::BACKGROUND_PARALLAX)))
		{
			editor->Push_Action({ static_cast<int>(background->m_Type), type, "backgroundType", handle, Action_General<BackgroundComponent, BACKGROUND_TYPE> });
			background->m_Type = static_cast<BACKGROUND_TYPE>(type);
		}

		if (RadioButton("Foreground##background_fg",   &type, static_cast<int>(BACKGROUND_TYPE::FOREGROUND)))
		{
			editor->Push_Action({ static_cast<int>(background->m_Type), type, "backgroundType", handle, Action_General<BackgroundComponent, BACKGROUND_TYPE> });
			background->m_Type = static_cast<BACKGROUND_TYPE>(type);
		}
		SameLine();
		if (RadioButton("Parallax (Foreground)##background_fg", &type, static_cast<int>(BACKGROUND_TYPE::FOREGROUND_PARALLAX)))
		{
			editor->Push_Action({ static_cast<int>(background->m_Type), type, "backgroundType", handle, Action_General<BackgroundComponent, BACKGROUND_TYPE> });
			background->m_Type = static_cast<BACKGROUND_TYPE>(type);
		}

		if (TreeNode("Parallax Bounds"))
		{
			Text("Minimum Point");
			Drag_Vec("X##background_parallax_min", bgSave.m_ParallaxBounds, background->m_ParallaxBounds.x, background->m_ParallaxBounds);
			Drag_Vec("Y##background_parallax_min", bgSave.m_ParallaxBounds, background->m_ParallaxBounds.y, background->m_ParallaxBounds);

			DragRelease(BackgroundComponent, bgSave.m_ParallaxBounds, background->m_ParallaxBounds, "parallaxBounds");


			Text("Maximum Point");
			Drag_Vec("X##background_parallax_max", bgSave.m_ParallaxBounds, background->m_ParallaxBounds.z, background->m_ParallaxBounds);
			Drag_Vec("Y##background_parallax_max", bgSave.m_ParallaxBounds, background->m_ParallaxBounds.w, background->m_ParallaxBounds);

			DragRelease(BackgroundComponent, bgSave.m_ParallaxBounds, background->m_ParallaxBounds, "parallaxBounds");
			
			Separator();
			TreePop();
		}

		if (TreeNode("Texture Size"))
		{
			Drag_Vec("X##background_texturesize", bgSave.m_SubTextureSize, background->m_SubTextureSize.x, background->m_SubTextureSize);
			Drag_Vec("Y##background_texturesize", bgSave.m_SubTextureSize, background->m_SubTextureSize.y, background->m_SubTextureSize);

			DragRelease(BackgroundComponent, bgSave.m_SubTextureSize, background->m_SubTextureSize, "subTextureSize");

			Separator();
			TreePop();
		}

		if (TreeNode("Texture Position"))
		{
			Drag_Vec("X##background_texturesize", bgSave.m_SubTexturePosition, background->m_SubTexturePosition.x, background->m_SubTexturePosition);
			Drag_Vec("Y##background_texturesize", bgSave.m_SubTexturePosition, background->m_SubTexturePosition.y, background->m_SubTexturePosition);

			DragRelease(BackgroundComponent, bgSave.m_SubTexturePosition, background->m_SubTexturePosition, "subTexturePosition");

			Separator();
			TreePop();
		}

		ResourceManager& rm = engine->GetResourceManager();
		std::vector<Resource *> sprites = rm.GetResourcesOfTypes_Alphabetical(ResourceType::TEXTURE, ResourceType::ANIMATION);

		Separator();
		
		
		if (Button("Reset##background_reset"))
		{
			editor->Push_Action({ bgSave.m_resID, -1, "resourceID", handle, Action_General<BackgroundComponent, ResourceID> });
			background->m_resID = -1;
		}
		SameLine();
		editor->GetSearchBars().background.Draw("Search", 100.0f);
		
		BeginChild("Sprites", SPRITE_ASSETS_LIST_SIZE, true);
		for (auto resource : sprites)
		{
			if (resource->Id() == background->m_resID)
			{
				PushStyleColor(ImGuiCol_Header, SPRITE_SELECTED_COLOR);
				Selectable(resource->FileName().c_str(), true);
				PopStyleColor();
				continue;
			}

			if (editor->GetSearchBars().background.PassFilter(resource->FileName().c_str()))
			{
				if (Selectable(resource->FileName().c_str()))
				{
					// Is resource ref counted, can I store pointers to them?
					editor->Push_Action({ background->m_resID, resource->Id(), "resourceID", handle, Action_General<BackgroundComponent, ResourceID> });
					background->SetResource(resource);
				}
			}
		}
		EndChild();
	}
}


void ImGui_Text(TextComponent *text, GameObject object, Editor *editor)
{
	if (CollapsingHeader("Text##text_component"))
	{
		
	}
}

// Text Component

// Light Component
// UI Component


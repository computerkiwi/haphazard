/*
FILE: Type_Binds.cpp
PRIMARY AUTHOR: Sweet

Copyright � 2017 DigiPen (USA) Corporation.
*/

#include "Editor.h"
#include "Type_Binds.h"

#include "GameObjectSystem/GameObject.h"
#include "GameObjectSystem\GameSpace.h"
#include "GameObjectSystem\ObjectInfo.h"
#include "Engine\Physics\RigidBody.h"
#include "graphics\SpriteComponent.h"
#include "graphics\Particles.h"
#include "Engine\Physics\Collider2D.h"
#include "Scripting\ScriptComponent.h"
#include "graphics\Camera.h"

#include "graphics\DebugGraphic.h"

#include "Input\Input.h"

#include <cfloat>

using namespace ImGui;

#define GAMEOBJECT_WINDOW_SIZE ImVec2(375, 600)
#define GAMEOBJECT_WINDOW_POS  ImVec2(1200, 30)

template <class Component, typename T>
void Action_General(EditorAction& a)
{
	ComponentHandle<Component> handle(a.handle);
	meta::Any obj(handle.Get());

	if (a.redo)
	{
		obj.SetPointerMember(a.name, a.current.GetData<T>());
	}
	else
	{
		obj.SetPointerMember(a.name, a.save.GetData<T>());
	}
}


template <>
void Action_General<SpriteComponent, ResourceID>(EditorAction& a)
{
	ComponentHandle<SpriteComponent> handle(a.handle);

	if (a.redo)
	{
		handle->SetTextureID(a.current.GetData<ResourceID>());
	}
	else
	{
		handle->SetTextureID(a.save.GetData<ResourceID>());
	}
}


void Action_General_Tags(EditorAction& a)
{
	ComponentHandle<ObjectInfo> handle(a.handle);
	if (a.redo)
	{
		handle->AddTag(a.current.GetData<std::string>().c_str());
	}
	else
	{
		handle->m_tags.erase(a.save.GetData<std::size_t>());
	}
}


void Action_General_Collider(EditorAction& a)
{
	ComponentHandle<DynamicCollider2DComponent> handle(a.handle);

	meta::Any colliderData(&handle->ColliderData());

	if (a.redo)
	{
		colliderData.SetPointerMember(a.name, a.current.GetData<glm::vec3>());
	}
	else
	{
		colliderData.SetPointerMember(a.name, a.save.GetData<glm::vec3>());
	}
}


template <class Component>
void Action_DeleteComponent(EditorAction& a)
{
	ComponentHandle<Component> handle(a.handle);

	if (a.redo)
	{
		handle.GetGameObject().DeleteComponent<Component>();
	}
	else
	{
		handle.GetGameObject().AddComponent<Component>(std::move(a.save.GetData<Component>()));
	}
}



#define Push_AddComponent(TYPE) editor->Push_Action({ 0, 0, nullptr, { object.Getid(), true }, Action_AddComponent<TYPE> });
template <class Component>
void Action_AddComponent(EditorAction& a)
{
	ComponentHandle<Component> handle(a.handle);

	if (a.redo)
	{
		handle.GetGameObject().AddComponent<Component>();
	}
	else
	{
		handle.GetGameObject().DeleteComponent<Component>();
	}
}


template <>
void Action_AddComponent<DynamicCollider2DComponent>(EditorAction& a)
{
	ComponentHandle<DynamicCollider2DComponent> handle(a.handle);

	if (a.redo)
	{
		handle.GetGameObject().AddComponent<DynamicCollider2DComponent>();

		if (a.save.GetData<bool>())
		{
			handle.GetGameObject().AddComponent<RigidBodyComponent>();
		}
	}
	else
	{
		handle.GetGameObject().DeleteComponent<DynamicCollider2DComponent>();

		if (a.save.GetData<bool>())
		{
			handle.GetGameObject().DeleteComponent<RigidBodyComponent>();
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


struct EditorBoolWrapper
{
	bool value = false;
	operator bool() const { return value; }
	EditorBoolWrapper& operator=(bool val) { value = val; return *this; }
};

typedef std::map<const char *, EditorBoolWrapper> ClickedList;
ClickedList widget_click;

// Error Message for when adding a component that exist
#define HAS_COMPONENT editor->AddPopUp(PopUpWindow(ErrorList[HasComponent], 2.0f, PopUpPosition::Mouse))

// Button to release the drag sliders
#define Drag_Key Key::Mouse_1

// Setups the detection of the drag slider has been clicked
// Saves a float in a temporary value then stores the new value and the saved value
// Undo pops the saved value
// Redo pops the new value
#define Drag(NAME, SAVE, ITEM)																				 \
	if (DragFloat_ReturnOnClick(NAME, &ITEM, SLIDER_STEP))													 \
	{																										 \
		if (widget_click[#SAVE] == false)																			 \
		{																									 \
			SAVE = ITEM;																					 \
			widget_click[#SAVE] = true;																				 \
		}																									 \
	}																										 

// Same as Drag but it saves a vector instead of a float
#define Drag_Vec(NAME, SAVE, ITEM, VEC)																		 \
	if (DragFloat_ReturnOnClick(NAME, &ITEM, SLIDER_STEP))													 \
	{																										 \
		if (widget_click[#SAVE] == false)																			 \
		{																									 \
			SAVE = VEC;																						 \
			widget_click[#SAVE] = true;																				 \
		}																									 \
	}

// Drag_Vec with min and max
#define Drag_Vec_MinMax(NAME, SAVE, ITEM, VEC, MIN, MAX)																		 \
	if (DragFloat_ReturnOnClick(NAME, &ITEM, SLIDER_STEP, MIN, MAX))													 \
	{																										 \
		if (widget_click[#SAVE] == false)																			 \
		{																									 \
			SAVE = VEC;																						 \
			widget_click[#SAVE] = true;																				 \
		}																									 \
	}

#define Drag_Float_Speed(NAME, SAVE, ITEM, SPEED)															 \
	if (DragFloat_ReturnOnClick(NAME, &ITEM, SPEED))														 \
	{																										 \
		if (widget_click[#SAVE] == false)																			 \
		{																									 \
			SAVE = ITEM;																					 \
			widget_click[#SAVE] = true;																				 \
		}																									 \
	}

#define Drag_Float_Speed_MinMax(NAME, SAVE, ITEM, SPEED, MIN, MAX)															 \
	if (DragFloat_ReturnOnClick(NAME, &ITEM, SPEED, MIN, MAX))														 \
	{																										 \
		if (widget_click[#SAVE] == false)																			 \
		{																									 \
			SAVE = ITEM;																					 \
			widget_click[#SAVE] = true;																				 \
		}																									 \
	}

#define Drag_Int(NAME, SAVE, ITEM)																			 \
	if (DragInt_ReturnOnClick(NAME, &ITEM, SLIDER_STEP))													 \
	{																										 \
		if (widget_click[#SAVE] == false)																			 \
		{																									 \
			SAVE = ITEM;																					 \
			widget_click[#SAVE] = true;																				 \
		}																									 \
	}

#define Drag_Int_Speed(NAME, SAVE, ITEM, SPEED)																 \
	if (DragInt_ReturnOnClick(NAME, &ITEM, SPEED))															 \
	{																										 \
		if (widget_click[#SAVE] == false)																			 \
		{																									 \
			SAVE = ITEM;																					 \
			widget_click[#SAVE] = true;																				 \
		}																									 \
	}

#define Drag_Int_Speed_MinMax(NAME, SAVE, ITEM, SPEED, MIN, MAX)																 \
	if (DragInt_ReturnOnClick(NAME, &ITEM, SPEED, MIN, MAX))															 \
	{																										 \
		if (widget_click[#SAVE] == false)																			 \
		{																									 \
			SAVE = ITEM;																					 \
			widget_click[#SAVE] = true;																				 \
		}																									 \
	}

#define DragRelease(COMPONENT, SAVE, ITEM, META_NAME)														 \
	if (Input::IsReleased(Drag_Key) && widget_click[#SAVE] == true)													 \
	{																										 \
		editor->Push_Action({ SAVE, ITEM,  META_NAME, handle, Action_General<COMPONENT, decltype(ITEM)> });  \
		widget_click[#SAVE] = false;																				 \
	}


#define DragRelease_Type(COMPONENT, SAVE, ITEM, META_NAME, TYPE)											 \
	if (Input::IsReleased(Drag_Key) && widget_click[#SAVE] == true)													 \
	{																										 \
		editor->Push_Action({ SAVE, ITEM,  META_NAME, handle, Action_General<COMPONENT, TYPE> });			 \
		widget_click[#SAVE] = false;																				 \
	}

#define DragRelease_Type_CastAll(COMPONENT, SAVE, ITEM, META_NAME, TYPE)											 \
	if (Input::IsReleased(Drag_Key) && widget_click[#SAVE] == true)													 \
	{																										 \
		editor->Push_Action({ TYPE(SAVE), TYPE(ITEM),  META_NAME, handle, Action_General<COMPONENT, TYPE> });			 \
		widget_click[#SAVE] = false;																				 \
	}

// Transform Component Save Location
TransformComponent transformSave;
RigidBodyComponent rigidBodySave;
Collider2D         colliderSave;

ParticleSettings   particleSave;

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


void Choose_Parent_ObjectList(Editor *editor, TransformComponent *transform, GameObject child)
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

		// Save the buffer based off name size, max name size is 8
		if (name.size() > 8)
		{
			snprintf(name_buffer, sizeof(name_buffer),
				"%-5.5s... - %d : %d", name.c_str(), object.GetObject_id(), object.GetIndex());
		}
		else
		{
			snprintf(name_buffer, sizeof(name_buffer),
				"%-8.8s - %d : %d", name.c_str(), object.GetObject_id(), object.GetIndex());
		}

		// Draw each object
		if (ImGui::Selectable(name_buffer))
		{
			transform->SetParent(object);
			if (object.GetComponent<HierarchyComponent>().IsValid())
			{
				object.GetComponent<HierarchyComponent>()->Add(child);
			}
			else
			{
				object.AddComponent<HierarchyComponent>(child);
			}	
		}
	}
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


		SetNextWindowSize(GAMEOBJECT_WINDOW_SIZE);
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
			object.Delete();
			editor->SetGameObject(0);
			End();
			return;
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
			Separator();
			if (Button("RigidBody", COMPONENT_BUTTON_SIZE))
			{
				if (object.GetComponent<RigidBodyComponent>().IsValid())
				{
					HAS_COMPONENT;
				}
				else
				{
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

						editor->Push_Action({ 0, added_rigidbody, nullptr,{ object.Getid(), true }, Action_AddComponent<DynamicCollider2DComponent> });
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
						object.AddComponent<StaticCollider2DComponent>(glm::vec3(1, 1, 0), collisionLayers::allCollision, Collider2D::colliderType::colliderBox);
						Push_AddComponent(StaticCollider2DComponent);
					}
				}
			}
			Separator();
			if (Button("Script", COMPONENT_BUTTON_SIZE))
			{
				if (object.GetComponent<ScriptComponent>().IsValid())
				{
					HAS_COMPONENT;
				}
				else
				{
					object.AddComponent<ScriptComponent>();
					Push_AddComponent(ScriptComponent);
				}
			}

			EndPopup();
		}


		if (Button("Add Component"))
		{
			OpenPopup("Components");
		}


		ImGui_ObjectInfo(object.GetComponent<ObjectInfo>().Get(), editor);


		// if object - > component
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


		if (object.GetComponent<SpriteComponent>().IsValid())
		{
			ImGui_Sprite(object.GetComponent<SpriteComponent>().Get(), object, editor);
		}

		if (object.GetComponent<ParticleSystem>().IsValid())
		{
			ImGui_Particles(object.GetComponent<ParticleSystem>().Get(), object, editor);
		}

		if (object.GetComponent<ScriptComponent>().IsValid())
		{
			ImGui_Script(object.GetComponent<ScriptComponent>().Get(), object, editor);
		}

		if (object.GetComponent<Camera>().IsValid())
		{
			ImGui_Camera(object.GetComponent<Camera>().Get(), object, editor);
		}

		End();
	}
	else
	{
		// No Object was selected so display a default
		const char *name = "GameObject - No Object Selected###GAMEOBJECT_ID";

		SetNextWindowSize(GAMEOBJECT_WINDOW_SIZE);
		SetNextWindowPos(GAMEOBJECT_WINDOW_POS, ImGuiCond_Once);
		Begin(name, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);
		End();
	}
}


void ImGui_GameObject_Multi(Array<GameObject_ID, MAX_SELECT>& objects, Editor *editor)
{
#if 1
	for (size_t i = 0; i < objects.m_size - 1; i++)
	{
		GameObject object = objects[i];
		
		TransformComponent *transform = object.GetComponent<TransformComponent>().Get();
		DebugGraphic::DrawShape(transform->GetPosition(), glm::vec2(transform->GetScale()) + glm::vec2(0.025f, 0.025f), (transform->GetRotation() * 3.14159265f) / 180, glm::vec4(0, 1, 1, 1));
	}

	ImGui_GameObject(objects[objects.m_size - 1], editor);

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

				editor->Push_Action({ hash(buffer), name_save, nullptr, handle, Action_General_Tags });

				CloseCurrentPopup();
			}

			EndPopup();
		}
	}
}

#define SLIDER_STEP 0.01f
#define ID_MASK 0xFFFFFF

// Binds the imgui function calls to the Transform Component
void ImGui_Transform(TransformComponent *transform, GameObject object, Editor *editor)
{
	// Draws the Selection Box
	glm::vec2 scale(transform->GetScale());
	DebugGraphic::DrawShape(transform->GetPosition(), scale + glm::vec2(0.025f, 0.025f), (transform->GetRotation() * 3.14159265f) / 180, glm::vec4(0,1,0,1));

	EditorComponentHandle handle = { object.Getid(), true };
		
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
			Text("Parent Object: %d | %s", transform->GetParent().Getid() & ID_MASK, transform->GetParent().GetComponent<ObjectInfo>()->m_name.c_str());
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
			Choose_Parent_ObjectList(editor, transform, object);
			EndPopup();
		}
	}

	if (TreeNode("Position"))
	{
		if (transform->GetParent())
		{
			Text("X: %f", transform->GetPosition().x);
			Text("Y: %f", transform->GetPosition().y);

			// Position Widgets
			Drag_Vec("X Offset##transform_position", transformSave.m_position, transform->m_position.x, transform->m_position);
			Drag_Vec("Y Offset##transform_position", transformSave.m_position, transform->m_position.y, transform->m_position);

			DragRelease_Type_CastAll(TransformComponent, transformSave.m_position, transform->m_position, "position", glm::vec2);
		}
		else
		{
			Drag_Vec("X##transform_position", transformSave.m_position, transform->m_position.x, transform->m_position);
			Drag_Vec("Y##transform_position", transformSave.m_position, transform->m_position.y, transform->m_position);

			DragRelease_Type_CastAll(TransformComponent, transformSave.m_position, transform->m_position, "position", glm::vec2);
		}

		TreePop();
		Separator();
	}
	if (TreeNode("Scale"))
	{

		Drag_Vec("X##scale", transformSave.m_scale, transform->m_scale.x, transform->m_scale);
		Drag_Vec("Y##scale", transformSave.m_scale, transform->m_scale.y, transform->m_scale);

		DragRelease(TransformComponent, transformSave.m_scale, transform->m_scale, "scale");
			
		TreePop();
		Separator();
	}

	int z_layer = static_cast<int>(transform->GetZLayer());
	if (InputInt("Z-Layer##transform", &z_layer, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		if (z_layer < 0)
		{
			z_layer = 0;
		}
		editor->Push_Action({ transform->m_position.z, z_layer, "zLayer", handle, Action_General<TransformComponent, float> });
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
		float mass = 1 / rb->Mass();

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
		InputFloat("Mass", &mass);
		rb->SetMass(1 / mass);
	}
}


// Binds the imgui function calls to the Sprite Component
void ImGui_Sprite(SpriteComponent *sprite, GameObject object, Editor * editor)
{
	if (CollapsingHeader("Sprite"))
	{
		EditorComponentHandle handle = { object.Getid(), true };
		if (Button("Remove##sprite"))
		{
			editor->Push_Action({ *sprite, 0, nullptr, handle, Action_DeleteComponent<SpriteComponent> });
			object.DeleteComponent<SpriteComponent>();
			return;
		}

		// if (sprite->isAnimated())
		// {
		//		int frame = sprite->GetFrame();
		//		SliderInt("Frame", &frame, 0, sprite->GetAnimatedTexture()->GetFrameCount());
		//		sprite->SetFrame(frame);
		// }


		ResourceManager& rm = engine->GetResourceManager();

		std::vector<Resource *> sprites = rm.GetResourcesOfType(ResourceType::TEXTURE);
		
		ResourceID id = sprite->GetResourceID();

		Separator();
		BeginChild("Sprites", ImVec2(0, 125), true);
		for (auto resource : sprites)
		{
			if (resource->Id() == id)
			{
				PushStyleColor(ImGuiCol_Header, ImVec4( 223/255.0f, 104/255.0f, 76/255.0f, 1.0f ));
				Selectable(resource->FileName().c_str(), true);
				PopStyleColor();
				continue;
			}
			if (Selectable(resource->FileName().c_str()))
			{
				// Is resource ref counted, can I store pointers to them?
				sprite->SetTextureResource(resource);
				editor->Push_Action({ id, resource->Id(), "resourceID", handle, Action_General<SpriteComponent, ResourceID> });
			}
		}
		EndChild();
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

		if (TreeNode("Dimensions"))
		{
			bool matchScale;
			Checkbox("Match Scale", &matchScale);

			if (matchScale)
			{
				colliderSave.m_dimensions = collider->m_dimensions;

				collider->m_dimensions = object.GetComponent<TransformComponent>()->GetScale();

				if (collider->isStatic())
				{
					editor->Push_Action({ colliderSave.m_dimensions, collider->m_dimensions, "dimensions",
						handle, Action_General_Collider });
				}
				else
				{
					editor->Push_Action({ colliderSave.m_dimensions, collider->m_dimensions, "dimensions",
						handle, Action_General_Collider });
				}
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
							handle, Action_General_Collider });
					}
					else
					{
						editor->Push_Action({ colliderSave.m_dimensions, collider->m_dimensions, "dimensions",
							handle, Action_General_Collider });
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
						editor->Push_Action({ colliderSave.m_offset, collider->m_offset, "offset", handle, Action_General_Collider });
					}
					else
					{
						editor->Push_Action({ colliderSave.m_offset, collider->m_offset, "offset", handle, Action_General_Collider });
					}
					widget_click["colliderSave.m_offset"] = false;
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
				index = i - 1;
				break;
			}
		}

		Drag_Float_Speed_MinMax("Elasticity##collider", colliderSave.m_selfElasticity, collider->m_selfElasticity, SLIDER_STEP, 0, 1);
		DragRelease(Collider2D, colliderSave.m_selfElasticity, collider->m_selfElasticity, "selfElasticity");

		// Collision Type
		Combo("Collider Type", &index, collider_types, static_cast<int>(Collider2D::colliderType::collider_max) - 2);
		switch (index)
		{
		case 0:
			collider->m_colliderShape = Collider2D::colliderType::colliderBox;
			break;

		default:
			break;
		};
		Separator();
		
		// Collision Layers
		if (TreeNode("Collision Layers"))
		{
			int layer = collider->m_collisionLayer;

			Columns(2, nullptr, false);
			RadioButton("All Collision",   &layer, collisionLayers::allCollision);   NextColumn();
			RadioButton("No Collision",    &layer, collisionLayers::noCollision);    NextColumn();
			RadioButton("Player",          &layer, collisionLayers::player);         NextColumn();
			RadioButton("Decor",		   &layer, collisionLayers::decor);		     NextColumn();
			RadioButton("Ground",		   &layer, collisionLayers::ground);	     NextColumn();
			RadioButton("Ally Projectile", &layer, collisionLayers::allyProjectile); NextColumn();
			RadioButton("Enemy",		   &layer, collisionLayers::enemy);
			Columns();

			// Check if we need to save the action for static or dynamic
			if (collider->isStatic())
			{
				editor->Push_Action({ collider->m_collisionLayer, layer, "collisionLayer",
								handle, Action_General<StaticCollider2DComponent, int> });
			}
			else
			{
				editor->Push_Action({ collider->m_collisionLayer, layer, "collisionLayer",
								handle, Action_General<DynamicCollider2DComponent, int> });
			}

			collider->m_collisionLayer = CollisionLayer(layer);

			TreePop();
		}
	}
}


bool ObjectHasScript(ResourceID resource, std::vector<LuaScript>& scripts)
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


// Binds the imgui function calls to the Script Component
void ImGui_Script(ScriptComponent *script_c, GameObject object, Editor * editor)
{
	if (CollapsingHeader("Script"))
	{
		EditorComponentHandle handle = { object.Getid(), true };
		ResourceManager& rm = engine->GetResourceManager();
		std::vector<Resource *> scripts = rm.GetResourcesOfType(ResourceType::SCRIPT);

		if (Button("Remove##script"))
		{
			editor->Push_Action({ *script_c, 0, nullptr, handle, Action_DeleteComponent<ScriptComponent> });
			object.DeleteComponent<ScriptComponent>();
			return;
		}
		Separator();
		if (Button("Add##script"))
		{
			OpenPopup("##script_add_script");
		}

		if (BeginPopup("##script_add_script"))
		{
			for (auto& script : scripts)
			{
				if (!ObjectHasScript(script->Id(), script_c->scripts))
				{
					if (Selectable(script->FileName().c_str()))
					{
						script_c->scripts.emplace_back(LuaScript(script, object));
					}
				}
			}

			EndPopup();
		}

		PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.44f, 0));
		for (int i = 0; i < script_c->scripts.size(); i++)
		{
			LuaScript& script = script_c->scripts[i];
			
			if (Button("X##script_remove_script", ImVec2(25, 0)))
			{
				script_c->scripts.erase(script_c->scripts.begin() + i);
				break;
			}
			SameLine();

			Text(rm.Get(script.GetResourceID())->FileName().c_str());
		}
		PopStyleVar();
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

		ParticleSettings& settings = particles->m_settings;
		
		Checkbox("Looping", &settings.isLooping);
		
		Drag_Float_Speed_MinMax("Rate##particles", particleSave.emissionRate, settings.emissionRate, SLIDER_STEP, 0.005f, FLT_MAX);
		DragRelease(ParticleSystem, particleSave.emissionRate, settings.emissionRate, "EmissionRate");

		Drag_Int_Speed_MinMax("Count##particles", particleSave.particlesPerEmission, settings.particlesPerEmission, 0.05f, 1, INT_MAX);
		DragRelease(ParticleSystem, particleSave.particlesPerEmission, settings.particlesPerEmission, "ParticlesPerEmission");


		if (TreeNode("Burst##particles"))
		{
			InputFloat("Frequency", &settings.burstEmission.z, SLIDER_STEP, 0);

			Drag_Vec_MinMax("Min Count##particle", particleSave.burstEmission, settings.burstEmission.x, settings.burstEmission, 0, FLT_MAX);
			Drag_Vec_MinMax("Max Count##particle", particleSave.burstEmission, settings.burstEmission.y, settings.burstEmission, 0, FLT_MAX);

			DragRelease(ParticleSettings, particleSave.burstEmission, settings.burstEmission, "BurstEmission");
			Separator();
			TreePop();
		}

		

		Combo("Shape##particles", reinterpret_cast<int *>(&settings.emissionShape), EmissionShape_Names, _countof(EmissionShape_Names));

		Separator();

		if (TreeNode("Shape Scale##particles"))
		{
			Drag_Vec("X##particle_emission_rate", particleSave.emissionShapeScale, settings.emissionShapeScale.x, settings.emissionShapeScale);
			Drag_Vec("Y##particle_emission_rate", particleSave.emissionShapeScale, settings.emissionShapeScale.y, settings.emissionShapeScale);

			DragRelease(ParticleSettings, particleSave.emissionShapeScale, settings.emissionShapeScale, "EmissionShapeScale");

			TreePop();
		}

		Separator();

		Combo("Simulation Space##particles", reinterpret_cast<int *>(&settings.particleSpace), SimulationSpace_Names, _countof(SimulationSpace_Names));

		Drag("Emitter Lifetime##particles", particleSave.emitterLifetime, settings.emitterLifetime);
		DragRelease(ParticleSettings, particleSave.emitterLifetime, settings.emitterLifetime, "EmitterLifetime");

		Drag("Lifetime##particles", particleSave.particleLifetime, settings.particleLifetime);
		DragRelease(ParticleSettings, particleSave.particleLifetime, settings.particleLifetime, "EmitterLifetime");

		Drag("Lifetime Variance##particles", particleSave.particleLifetimeVariance, settings.particleLifetimeVariance);
		DragRelease(ParticleSettings, particleSave.particleLifetimeVariance, settings.particleLifetimeVariance, "ParticleLifetimeVariance");

		Separator();

		if (TreeNode("Velocity##particles"))
		{
			Drag_Vec("X##particles_init_velocity", particleSave.startingVelocity, settings.startingVelocity.x, settings.startingVelocity);
			Drag_Vec("Y##particles_init_velocity", particleSave.startingVelocity, settings.startingVelocity.y, settings.startingVelocity);

			DragRelease(ParticleSettings, particleSave.startingVelocity, settings.startingVelocity, "StartingVelocity");

			if (TreeNode("Variance##particles"))
			{
				Drag_Vec("X##particles_variance_velocity", particleSave.startingVelocityVariance, settings.startingVelocityVariance.x, settings.startingVelocityVariance);
				Drag_Vec("Y##particles_variance_velocity", particleSave.startingVelocityVariance, settings.startingVelocityVariance.y, settings.startingVelocityVariance);

				DragRelease(ParticleSettings, particleSave.startingVelocityVariance, settings.startingVelocityVariance, "StartingVelocityVariance");

				TreePop();
			}
			Separator();
			TreePop();
		}

		if (TreeNode("Acceleration##particles"))
		{
			Drag_Vec("X##particles_acceleration", particleSave.acceleration, settings.acceleration.x, settings.acceleration);
			Drag_Vec("Y##particles_acceleration", particleSave.acceleration, settings.acceleration.y, settings.acceleration);

			DragRelease(ParticleSettings, particleSave.acceleration, settings.acceleration, "Acceleration");

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

			DragRelease(ParticleSettings, particleSave.scaleOverTime, settings.scaleOverTime, "ScaleOverTime");

			Separator();
			TreePop();
		}

		if (TreeNode("Rotation##rotation_particles"))
		{
			Drag("Start##particles_rotation", particleSave.startRotation, settings.startRotation);
			DragRelease(ParticleSystem, particleSave.startRotation, settings.startRotation, "StartRotation");


			Drag("Variance##particles_rot_variance", particleSave.startRotationVariation, settings.startRotationVariation);
			DragRelease(ParticleSystem, particleSave.startRotationVariation, settings.startRotationVariation, "StartRotationVariation");
			
			Drag("Rate##particles_rot_rate", particleSave.rotationRate, settings.rotationRate);
			DragRelease(ParticleSystem, particleSave.rotationRate, settings.rotationRate, "RotationRate");

			Separator();
			TreePop();
		}

		if (TreeNode("Sprite Data##particles"))
		{
			if (TreeNode("Color##particles"))
			{
				Text("Start Color");
				Drag_Vec_MinMax("R##particles_startColor", particleSave.startColor, settings.startColor.x, settings.startColor, 0, 1);
				Drag_Vec_MinMax("G##particles_startColor", particleSave.startColor, settings.startColor.y, settings.startColor, 0, 1);
				Drag_Vec_MinMax("B##particles_startColor", particleSave.startColor, settings.startColor.z, settings.startColor, 0, 1);
				Drag_Vec_MinMax("A##particles_startColor", particleSave.startColor, settings.startColor.w, settings.startColor, 0, 1);

				DragRelease(ParticleSystem, particleSave.startColor, settings.startColor, "StartColor");

				Separator();

				Text("End Color");
				Drag_Vec_MinMax("R##particles_endColor", particleSave.endColor, settings.endColor.x, settings.endColor, 0, 1);
				Drag_Vec_MinMax("G##particles_endColor", particleSave.endColor, settings.endColor.y, settings.endColor, 0, 1);
				Drag_Vec_MinMax("B##particles_endColor", particleSave.endColor, settings.endColor.z, settings.endColor, 0, 1);
				Drag_Vec_MinMax("A##particles_endColor", particleSave.endColor, settings.endColor.w, settings.endColor, 0, 1);

				DragRelease(ParticleSystem, particleSave.endColor, settings.endColor, "EndColor");


				TreePop();
			}

			if (TreeNode("Sprite"))
			{
				ResourceManager& rm = engine->GetResourceManager();

				std::vector<Resource *> sprites = rm.GetResourcesOfType(ResourceType::TEXTURE);

				Separator();
				BeginChild("Sprites", ImVec2(0, 125), true);
				for (auto resource : sprites)
				{
					if (resource->Id() == settings.texture_resourceID)
					{
						PushStyleColor(ImGuiCol_Header, ImVec4(223 / 255.0f, 104 / 255.0f, 76 / 255.0f, 1.0f));
						Selectable(resource->FileName().c_str(), true);
						PopStyleColor();
						continue;
					}
					if (Selectable(resource->FileName().c_str()))
					{
						// Is resource ref counted, can I store pointers to them?
						editor->Push_Action({ settings.texture_resourceID, resource->Id(), "TextureResourceID", handle, Action_General<ParticleSystem, ResourceID> });
						settings.texture_resourceID = resource->Id();
					}
				}
				EndChild();

				Separator();
				TreePop();
			}

			Separator();
			TreePop();
		}

		if (TreeNode("Trail##particles"))
		{
			if (Checkbox("Has Trail##particles", &settings.hasTrail))
			{
				editor->Push_Action({ !settings.hasTrail, settings.hasTrail, "HasTrail", handle, Action_General<ParticleSystem, bool> });
			}

			Drag("Rate##particles_trail", particleSave.trailEmissionRate, settings.trailEmissionRate);
			DragRelease(ParticleSystem, particleSave.trailEmissionRate, settings.trailEmissionRate, "TrailEmissionRate");

			Drag("Lifetime##particles_trail", particleSave.trailLifetime, settings.trailLifetime);
			DragRelease(ParticleSystem, particleSave.trailLifetime, settings.trailLifetime, "TrailLifetime");

			if (TreeNode("Color##trail_particles"))
			{
				Text("Start Color");
				Drag_Vec("R##particles_trail_startColor", particleSave.trailStartColor, settings.trailStartColor.x, settings.trailStartColor);
				Drag_Vec("G##particles_trail_startColor", particleSave.trailStartColor, settings.trailStartColor.y, settings.trailStartColor);
				Drag_Vec("B##particles_trail_startColor", particleSave.trailStartColor, settings.trailStartColor.z, settings.trailStartColor);
				Drag_Vec("A##particles_trail_startColor", particleSave.trailStartColor, settings.trailStartColor.w, settings.trailStartColor);
				
				DragRelease(ParticleSystem, particleSave.trailStartColor, settings.trailStartColor, "TrailStartColor");

				Separator();

				Text("End Color");
				Drag_Vec("R##particles_trail_endColor", particleSave.trailEndColor, settings.trailEndColor.x, settings.trailEndColor);
				Drag_Vec("G##particles_trail_endColor", particleSave.trailEndColor, settings.trailEndColor.y, settings.trailEndColor);
				Drag_Vec("B##particles_trail_endColor", particleSave.trailEndColor, settings.trailEndColor.z, settings.trailEndColor);
				Drag_Vec("A##particles_trail_endColor", particleSave.trailEndColor, settings.trailEndColor.w, settings.trailEndColor);

				DragRelease(ParticleSystem, particleSave.trailEndColor, settings.trailEndColor, "TrailEndColor");

				TreePop();
			}

			Separator();
			TreePop();
		}
	}
}


void ImGui_Camera(Camera *camera, GameObject object, Editor *editor)
{
	if (CollapsingHeader("Camera"))
	{
		EditorComponentHandle handle = { object.Getid(), true };

		Drag_Float_Speed_MinMax("Zoom##camera", cameraSave.m_Zoom, camera->m_Zoom, SLIDER_STEP, 0, FLT_MAX);
		DragRelease(Camera, cameraSave.m_Zoom, camera->m_Zoom, "zoom");
	}
}


// Background Component
// Text Component

// Light Component
// UI Component


/*
FILE: Type_Binds.cpp
PRIMARY AUTHOR: Sweet

Copyright � 2017 DigiPen (USA) Corporation.
*/

#include "Editor.h"
#include "Type_Binds.h"

#include "GameObjectSystem/GameObject.h"
#include "GameObjectSystem\GameSpace.h"
#include "Engine\Physics\RigidBody.h"
#include "graphics\SpriteComponent.h"
#include "Engine\Physics\Collider2D.h"
#include "Scripting\ScriptComponent.h"

using namespace ImGui;

#define GAMEOBJECT_WINDOW_SIZE ImVec2(375, 600)
#define GAMEOBJECT_WINDOW_POS  ImVec2(15, 20)

template <typename T>
void Action_General(EditorAction& a)
{
	*reinterpret_cast<T *>(a.object) = a.old_value.GetData<T>();
}

template <>
void Action_General<ResourceID>(EditorAction& a)
{
	static_cast<SpriteComponent *>(a.object)->SetTextureResource()
}


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
			
			if (InputText("Edit Name", name_buffer, 128, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				object.GetComponent<ObjectInfo>()->m_name = name_buffer;
			}

			EndPopup();
		}


		SameLine();

		// Add Component Buttons
		if (BeginPopup("Components"))
		{

			// Everything has a transform
			// Everything is going to have a data component
			
			if (Button("Sprite"))
			{
				if (object.GetComponent<SpriteComponent>().IsValid())
				{
				}
				else
				{
					object.AddComponent<SpriteComponent>();
				}
			}
			else if (Button("RigidBody"))
			{
				if (object.GetComponent<RigidBodyComponent>().IsValid())
				{
				}
				else
				{
					object.AddComponent<RigidBodyComponent>();
				}
			}
			else if (Button("Dynamic Collider"))
			{
				if (object.GetComponent<DynamicCollider2DComponent>().IsValid())
				{
				}
				else
				{
					if (object.GetComponent<StaticCollider2DComponent>().IsValid())
					{
						// Display an error
					}
					else
					{
						object.AddComponent<DynamicCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(1, 1, 0));
					}
				}
			}
			else if (Button("Static Collider"))
			{
				if (object.GetComponent<StaticCollider2DComponent>().IsValid())
				{
				}
				else
				{
					if (object.GetComponent<DynamicCollider2DComponent>().IsValid() || object.GetComponent<RigidBodyComponent>().IsValid())
					{
						// Display an error
					}
					else
					{
						object.AddComponent<StaticCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(1, 1, 0));
					}
				}
			}
			else if (Button("Script"))
			{
				if (object.GetComponent<ScriptComponent>().IsValid())
				{
				}
				else
				{
					object.AddComponent<ScriptComponent>();
				}
			}

			EndPopup();
		}


		if (Button("Add"))
		{
			OpenPopup("Components");
		}


		ImGui_ObjectInfo(object.GetComponent<ObjectInfo>().Get());


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
			ImGui_Collider2D(&object.GetComponent<StaticCollider2DComponent>().Get()->ColliderData(), object, editor);
		}
		else if (object.GetComponent<DynamicCollider2DComponent>().IsValid())
		{
			ImGui_Collider2D(&object.GetComponent<DynamicCollider2DComponent>().Get()->ColliderData(), object, editor);
		}


		if (object.GetComponent<SpriteComponent>().IsValid())
		{
			ImGui_Sprite(object.GetComponent<SpriteComponent>().Get(), object, editor);
		}

		if (object.GetComponent<ScriptComponent>().IsValid())
		{
			ImGui_Script(object.GetComponent<ScriptComponent>().Get(), object, editor);
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
#if 0
	for (size_t i = 0; i < objects.m_size; i++)
	{
		GameObject object = objects[i];
		(void)object;
	}
#else
	(void)objects;
	(void)editor;
#endif
}


//
// Component ImGui stuff
// ----------------------

void ImGui_ObjectInfo(ObjectInfo *info)
{
	if (info)
	{
		Separator();
		Text("ID: %d | %s", info->m_id & 0xFFFFFF, info->m_name.c_str());
	}
}

#define SLIDER_STEP 0.01f
#define ID_MASK 0xFFFFFF

void ImGui_Transform(TransformComponent *transform, GameObject object, Editor *editor)
{
	if (CollapsingHeader("Transform"))
	{
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

			int parent_id = 0;
			if (BeginPopup("Add Parent##add_parent_popup"))
			{
				Choose_Parent_ObjectList(editor, transform, object);
				EndPopup();
			}
		}

		if (TreeNode("Position"))
		{
			glm::vec2 position = transform->m_position;
			if (transform->GetParent())
			{
				bool x_click = false;
				bool y_click = false;

				Text("X: %f", transform->GetPosition().x);
				Text("Y: %f", transform->GetPosition().y);

				if (DragFloat("X Offset##position_drag", &transform->m_position.x, SLIDER_STEP, 0))
				{
					x_click = true;
				}
				else
				{
					x_click = false;
				}

				if (!x_click && position.x != transform->m_position.x)
				{
					editor->Push_Action({ position.x, transform->m_position.x, &transform->m_position.x, Action_General<float> });
					x_click = false;
				}

				DragFloat("Y Offset##position_drag", &transform->m_position.y, SLIDER_STEP, 0);
			}
			else
			{
				DragFloat("X##position_drag", &transform->GetRelativePosition().x, SLIDER_STEP, 0);
				DragFloat("Y##position_drag", &transform->GetRelativePosition().y, SLIDER_STEP, 0);
			}

			TreePop();
			Separator();
		}
		if (TreeNode("Scale"))
		{
			glm::vec2 scale = transform->m_scale;
			PushItemWidth(120);

			if (InputFloat("X##scale", &transform->m_scale.x, 0.0f, 0.0f, -1, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				editor->Push_Action({ scale.x, transform->m_scale.x, &transform->m_scale.x, Action_General<float> });
			}

			if (InputFloat("Y##scale", &transform->m_scale.y, 0.0f, 0.0f, -1, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				editor->Push_Action({ scale.y, transform->m_scale.y, &transform->m_scale.y, Action_General<float> });
			}

			TreePop();
			Separator();
		}

		DragFloat("##rotation_drag", &transform->m_rotation, 1, 0);
	}
}


void ImGui_RigidBody(RigidBodyComponent *rb, GameObject object, Editor * editor)
{
	if (CollapsingHeader("RigidBody"))
	{
		float mass = 1 / rb->Mass();

		if (Button("Remove##rigidbody"))
		{
			object.DeleteComponent<RigidBodyComponent>();
			return;
		}

		if (TreeNode("Acceleration"))
		{
			glm::vec2 acc = rb->m_acceleration;
			PushItemWidth(120);
			
			if (InputFloat(" X##acceleration", &rb->m_acceleration.x))
			{
				editor->Push_Action({ acc.x, rb->m_acceleration.x, &rb->m_acceleration.x, Action_General<float> });
			}

			if (InputFloat(" Y##acceleration", &rb->m_acceleration.y))
			{
				editor->Push_Action({ acc.y, rb->m_acceleration.y, &rb->m_acceleration.y, Action_General<float> });
			}
			
			
			PopItemWidth();
			TreePop();
			Separator();
		}
		if (TreeNode("Velocity"))
		{
			glm::vec2 vel = rb->m_velocity;
			PushItemWidth(120);

			if (InputFloat(" X##velocity", &rb->m_velocity.x))
			{
				editor->Push_Action({ vel.x, rb->m_velocity.x, &rb->m_velocity.x, Action_General<float> });
			}

			if (InputFloat(" Y##velocity", &rb->m_velocity.y))
			{
				editor->Push_Action({ vel.y, rb->m_velocity.y, &rb->m_velocity.y, Action_General<float> });
			}
			
			PopItemWidth();
			TreePop();
			Separator();
		}
		if (TreeNode("Gravity"))
		{
			glm::vec2 gravity = rb->m_gravity;
			PushItemWidth(120);

			if (InputFloat(" X##gravity", &rb->m_gravity.x))
			{
				editor->Push_Action({ gravity.x, rb->m_gravity.x, &rb->m_gravity.x, Action_General<float> });
			}

			if (InputFloat(" Y##gravity", &rb->m_gravity.y))
			{
				editor->Push_Action({ gravity.y, rb->m_gravity.y, &rb->m_gravity.y, Action_General<float> });
			}
			
			PopItemWidth();
			TreePop();
			Separator();
		}
		InputFloat("Mass", &mass);
		rb->SetMass(1 / mass);
	}
}


void ImGui_Sprite(SpriteComponent *sprite, GameObject object, Editor * editor)
{
	if (CollapsingHeader("Sprite"))
	{
		if (Button("Remove##sprite"))
		{
			object.DeleteComponent<SpriteComponent>();
			return;
		}

		ResourceManager& rm = engine->GetResourceManager();

		std::vector<Resource *> sprites = rm.GetResourcesOfType(ResourceType::TEXTURE);
		
		ResourceID id = sprite->GetResourceID();

		std::string name = rm.Get(id)->FileName();

		Text("Image Source: %s", name.c_str());

		for (auto resouce : sprites)
		{
			if (Selectable(resouce->FileName().c_str()))
			{
				sprite->SetResourceID(resouce->Id());
				editor->Push_Action({ id, sprite->m_resID, sprite, Action_General<ResourceID> });
			}
		}
	}
}


void ImGui_Collider2D(Collider2D *collider, GameObject object, Editor * editor)
{
	if (CollapsingHeader("Collider"))
	{
		if (Button("Remove##collider"))
		{
			if (collider->isStatic())
			{
				object.DeleteComponent<StaticCollider2DComponent>();
			}
			else
			{
				object.DeleteComponent<DynamicCollider2DComponent>();
			}
			return;
		}

		if (TreeNode("Dimensions"))
		{
			PushItemWidth(120);
			InputFloat(" X##dim", &collider->m_dimensions.x);
			InputFloat(" Y##dim", &collider->m_dimensions.y);
			PopItemWidth();
			TreePop();
			Separator();
		}
		if (TreeNode("Offset"))
		{
			PushItemWidth(120);
			InputFloat(" X##offset", &collider->m_offset.x);
			InputFloat(" Y##offset", &collider->m_offset.y);
			PopItemWidth();
			TreePop();
			Separator();
		}
		int index = collider->m_colliderType - 2;
		Combo("Collider Type", &index, collider_types, static_cast<int>(Collider2D::colliderType::collider_max) - 2);
		collider->m_colliderType = index;
	}
}


void ImGui_Script(ScriptComponent *script_c, GameObject object, Editor * editor)
{
	if (CollapsingHeader("Script"))
	{
		if (Button("Remove##script"))
		{
			object.DeleteComponent<ScriptComponent>();
			return;
		}
		char buffer[2048];
		if (InputText("", buffer, 2048, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			SameLine();
			if (Button("Add##script"))
			{
				script_c->scripts.emplace_back(LuaScript(buffer));
			}
		}

		for (auto& script : script_c->scripts)
		{
			(void)script;
			Text("SomeDrive:/Folder/Wow/Harambe/scripts/die.lua");
		}
	}
}



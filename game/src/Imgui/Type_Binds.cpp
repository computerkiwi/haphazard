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
			object.SetSpace(0);
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
				if (object.GetComponent<SpriteComponent>().Get())
				{
				}
				else
				{
					object.AddComponent<SpriteComponent>();
				}
			}
			else if (Button("RigidBody"))
			{
				if (object.GetComponent<RigidBodyComponent>().Get())
				{
				}
				else
				{
					object.AddComponent<RigidBodyComponent>();
				}
			}
			else if (Button("Dynamic Collider"))
			{
				if (object.GetComponent<DynamicCollider2DComponent>().Get())
				{
				}
				else
				{
					if (object.GetComponent<StaticCollider2DComponent>().Get())
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
				if (object.GetComponent<StaticCollider2DComponent>().Get())
				{
				}
				else
				{
					if (object.GetComponent<DynamicCollider2DComponent>().Get() || object.GetComponent<RigidBodyComponent>().Get())
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
				if (object.GetComponent<ScriptComponent>().Get())
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
			ImGui_RigidBody(object.GetComponent<RigidBodyComponent>().Get(), object);

			if (object.GetComponent<DynamicCollider2DComponent>().IsValid())
			{
				ImGui_Collider2D(&object.GetComponent<DynamicCollider2DComponent>().Get()->ColliderData(), object);
			}
		}
		else if (object.GetComponent<StaticCollider2DComponent>().IsValid())
		{
			ImGui_Collider2D(&object.GetComponent<StaticCollider2DComponent>().Get()->ColliderData(), object);
		}
		else if (object.GetComponent<DynamicCollider2DComponent>().IsValid())
		{
			ImGui_Collider2D(&object.GetComponent<DynamicCollider2DComponent>().Get()->ColliderData(), object);
		}


		if (object.GetComponent<SpriteComponent>().IsValid())
		{
			ImGui_Sprite(object.GetComponent<SpriteComponent>().Get(), object);
		}

		if (object.GetComponent<ScriptComponent>().IsValid())
		{
			ImGui_Script(object.GetComponent<ScriptComponent>().Get(), object);
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
				char name_buffer[128] = { 0 };

				for (auto& id : editor->m_objects)
				{
					GameObject object = id;

					if (object.Getid() == editor->m_selected_object)
					{
						continue;
					}

					std::string& name = object.GetComponent<ObjectInfo>().Get()->m_name;

					if (name.size() > 8)
					{
						snprintf(name_buffer, sizeof(name_buffer),
							"%-8.8s... - %d : %d", name.c_str(), object.Getid() & ID_MASK, object.GetIndex());
					}
					else
					{
						snprintf(name_buffer, sizeof(name_buffer),
							"%-8.8s    - %d : %d", name.c_str(), object.Getid() & ID_MASK, object.GetIndex());
					}

					

					if (Selectable(name_buffer))
					{
						transform->SetParent(id);
						break;
					}
				}
				EndPopup();
			}
		}

		if (TreeNode("Position"))
		{
			if (transform->GetParent())
			{
				Text("X: %f", transform->GetPosition().x);
				Text("Y: %f", transform->GetPosition().y);

				DragFloat("X Offset##position_drag", &transform->m_position.x, SLIDER_STEP, 0);
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
			PushItemWidth(120);
			InputFloat("X##scale", &transform->m_scale.x);
			InputFloat("Y##scale", &transform->m_scale.y);
			TreePop();
			Separator();
		}

		DragFloat("##rotation_drag", &transform->m_rotation, 1, 0);
	}
}


void ImGui_RigidBody(RigidBodyComponent *rb, GameObject object)
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
			PushItemWidth(120);
			InputFloat(" X##acceleration", &rb->m_acceleration.x);
			InputFloat(" Y##acceleration", &rb->m_acceleration.y);
			PopItemWidth();
			TreePop();
			Separator();
		}
		if (TreeNode("Velocity"))
		{
			PushItemWidth(120);
			InputFloat(" X##velocity", &rb->m_velocity.x);
			InputFloat(" Y##velocity", &rb->m_velocity.y);
			PopItemWidth();
			TreePop();
			Separator();
		}
		if (TreeNode("Gravity"))
		{
			PushItemWidth(120);
			InputFloat(" X##gravity", &rb->m_gravity.x);
			InputFloat(" Y##gravity", &rb->m_gravity.y);
			PopItemWidth();
			TreePop();
			Separator();
		}
		InputFloat("Mass", &mass);
		rb->SetMass(1 / mass);
	}
}


void ImGui_Sprite(SpriteComponent *sprite, GameObject object)
{
	if (CollapsingHeader("Sprite"))
	{
		if (Button("Remove##sprite"))
		{
			object.DeleteComponent<SpriteComponent>();
			return;
		}
		char buffer[2048] = { 0 };
		if (InputText("Image Source", buffer, 2048, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			sprite->SetTexture(&Texture(buffer));
		}
	}
}


void ImGui_Collider2D(Collider2D *collider, GameObject object)
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


void ImGui_Script(ScriptComponent *script_c, GameObject object)
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



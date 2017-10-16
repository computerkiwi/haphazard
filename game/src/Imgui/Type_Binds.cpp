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
#define GAMEOBJECT_WINDOW_POS  ImVec2(0, 0)

void ImGui_GameObject(GameObject object)
{
	// Check if a nullptr was passed
	if (object && object.GetSpace())
	{
		// Display the object's id
		std::string name("GameObject - ");
		name += std::to_string(object.Getid());
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
			End();
			return;
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

		PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor(0.25f, 0.55f, 0.9f)));
		PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor(0.0f, 0.45f, 0.9f)));
		PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor(0.25f, 0.25f, 0.9f)));
		if (Button("Add"))
		{
			// TODO[SWEET]:: Fix this pop bs
			OpenPopup("Components");
		}
		PopStyleColor(3);


		ImGui_ObjectInfo(object.GetComponent<ObjectInfo>().Get());


		// if object - > component
		// ImGui_Component(ComponetType *component);
		if (object.GetComponent<TransformComponent>().IsValid())
		{
			ImGui_Transform(object.GetComponent<TransformComponent>().Get(), object);
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
		Text("ID: %d |", info->m_id); SameLine();
		Text("Name: %s", info->m_name.c_str());
	}
}


void ImGui_Transform(TransformComponent *transform, GameObject object)
{
	if (CollapsingHeader("Transform"))
	{
		if (TreeNode("Position"))
		{
			PushItemWidth(80);
			DragFloat("X##position_drag", &transform->Position().x, 0, 5); SameLine();
			InputFloat("X##position", &transform->Position().x);

			DragFloat("Y##position_drag", &transform->Position().y, 0, 5); SameLine();
			InputFloat("Y##position", &transform->Position().y);
			PopItemWidth();
			TreePop();
			Separator();
		}
		if (TreeNode("Scale"))
		{
			PushItemWidth(120);
			InputFloat("X##scale", &transform->Scale().x);
			InputFloat("Y##scale", &transform->Scale().y);
			PopItemWidth();
			TreePop();
			Separator();
		}

		PushItemWidth(120);
		DragFloat("##rotation_drag", &transform->Rotation(), 0, 360);
		SameLine();
		PopItemWidth();
		PushItemWidth(100);
		InputFloat("Rotation", &transform->Rotation(), 0.0f, 0.0f, 2);
		PopItemWidth();
	}
}


void ImGui_RigidBody(RigidBodyComponent *rb, GameObject object)
{
	float mass = 1 / rb->Mass();
	if (CollapsingHeader("RigidBody"))
	{
		if (Button("Remove"))
		{
			object.DeleteComponent<RigidBodyComponent>();
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
		if (Button("Remove"))
		{
			object.DeleteComponent<SpriteComponent>();
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
		if (Button("Remove"))
		{
			if (collider->isStatic())
			{
				object.DeleteComponent<StaticCollider2DComponent>();
			}
			else
			{
				object.DeleteComponent<DynamicCollider2DComponent>();
			}
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
		int index = collider->m_colliderType;
		Combo("Collider Type", reinterpret_cast<int *>(&index), collider_types, static_cast<int>(Collider2D::colliderType::collider_max));
		collider->m_colliderType = index;
	}
}


void ImGui_Script(ScriptComponent *script_c, GameObject object)
{
	if (CollapsingHeader("Script"))
	{
		if (Button("Remove"))
		{

		}
		char buffer[2048];
		if (InputText("", buffer, 2048))
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



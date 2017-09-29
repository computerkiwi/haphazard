/*
FILE: Type_Binds.cpp
PRIMARY AUTHOR: Sweet

Copyright © 2017 DigiPen (USA) Corporation.
*/

#include "Editor.h"
#include "Type_Binds.h"

#include "GameObjectSystem\GameSpace.h"
#include "Engine\Physics\RigidBody.h"
#include "graphics\SpriteComponent.h"
#include "Engine\Physics\Collider2D.h"

using namespace ImGui;

#define GAMEOBJECT_WINDOW_SIZE ImVec2(375, 600)
#define GAMEOBJECT_WINDOW_POS  ImVec2(0, 0)

void ImGui_GameObject(GameObject *object)
{
	if (object && object->GetSpace())
	{
		std::string name("GameObject - ");
		name += std::to_string(object->Getid());
		name += "###GAMEOBJECT_ID";


		SetNextWindowSize(GAMEOBJECT_WINDOW_SIZE);
		SetNextWindowPos(GAMEOBJECT_WINDOW_POS, ImGuiCond_Once);
		Begin(name.c_str(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);

		PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor(0.25f, 0.55f, 0.9f)));
		PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor(0.0f, 0.45f, 0.9f)));
		PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor(0.25f, 0.25f, 0.9f)));
		if (Button("Duplicate"))
		{
			object->Duplicate<dummy>();
		}
		PopStyleColor(3);
		SameLine();
		if (Button("Delete"))
		{
			object->Delete<dummy>();
			object->SetSpace<dummy>(nullptr);
			End();
			return;
		}
		SameLine();

		PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor(0.25f, 0.55f, 0.9f)));
		PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor(0.0f, 0.45f, 0.9f)));
		PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor(0.25f, 0.25f, 0.9f)));
		if (Button("Add"))
		{
			if (BeginPopup("Components"))
			{
				if (Button("Transform"))
				{

				}
				else if (Button("Sprite"))
				{

				}
				else if (Button("RigidBody"))
				{

				}
				else if (Button("Dynamic Collider"))
				{

				}
				else if (Button("Static Collider"))
				{

				}
			}
		}
		PopStyleColor(3);
		SameLine();

		PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor(0.25f, 0.55f, 0.9f)));
		PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor(0.0f, 0.45f, 0.9f)));
		PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor(0.25f, 0.25f, 0.9f)));
		if (Button("Remove"))
		{
			if (BeginPopup("Components"))
			{
				if (object->GetComponent<TransformComponent>().IsValid())
				{
					if (Button("Transform"))
					{

					}
				}
				if (object->GetComponent<RigidBodyComponent>().IsValid())
				{
					if (Button("RigidBody"))
					{

					}
				}
				if (object->GetComponent<DynamicCollider2DComponent>().IsValid())
				{
					if (Button("Dynamic Collider"))
					{

					}
				}
				if (object->GetComponent<StaticCollider2DComponent>().IsValid())
				{
					if (Button("Static Collider"))
					{

					}
				}
			}
		}
		PopStyleColor(3);

		// if object - > component
		// ImGui_Component(ComponetType *component);
		if (object->GetComponent<TransformComponent>().IsValid())
		{
			ImGui_Transform(object->GetComponent<TransformComponent>().Get());
		}

		if (object->GetComponent<RigidBodyComponent>().IsValid())
		{
			ImGui_RigidBody(object->GetComponent<RigidBodyComponent>().Get());

			if (object->GetComponent<DynamicCollider2DComponent>().IsValid())
			{
				ImGui_Collider2D(&object->GetComponent<DynamicCollider2DComponent>().Get()->ColliderData());
			}
		}
		else if (object->GetComponent<StaticCollider2DComponent>().IsValid())
		{
			ImGui_Collider2D(&object->GetComponent<StaticCollider2DComponent>().Get()->ColliderData());
		}

		

		if (object->GetComponent<SpriteComponent>().IsValid())
		{
			ImGui_Sprite(object->GetComponent<SpriteComponent>().Get());
		}

		End();
	}
	else
	{
		const char *name = "GameObject - No Object Selected###GAMEOBJECT_ID";

		SetNextWindowSize(GAMEOBJECT_WINDOW_SIZE);
		SetNextWindowPos(GAMEOBJECT_WINDOW_POS);
		Begin(name, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove);
		End();
	}
}


void ImGui_Transform(TransformComponent *transform)
{
	if (CollapsingHeader("Transform"))
	{
		if (TreeNode("Position"))
		{
			PushItemWidth(120);
			InputFloat(" X##position", &transform->Position().x);
			InputFloat(" Y##position", &transform->Position().y);
			PopItemWidth();
			TreePop();
			Separator();
		}
		if (TreeNode("Scale"))
		{
			PushItemWidth(120);
			InputFloat(" X##scale", &transform->Scale().x);
			InputFloat(" Y##scale", &transform->Scale().y);
			PopItemWidth();
			TreePop();
			Separator();
		}

		PushItemWidth(120);
		DragFloat("", &transform->Rotation(), 0, 360);
		SameLine();
		PopItemWidth();
		PushItemWidth(100);
		InputFloat("Rotation", &transform->Rotation(), 0.0f, 0.0f, 2);
		PopItemWidth();
	}
}


void ImGui_RigidBody(RigidBodyComponent *rb)
{
	float mass = 1 / rb->Mass();
	if (CollapsingHeader("RigidBody"))
	{
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


void ImGui_Sprite(SpriteComponent *sprite)
{
	if (CollapsingHeader("Sprite"))
	{
		char buffer[2048] = { 0 };
		if (InputText("Image Source", buffer, 2048, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			sprite->SetTexture(&Texture(buffer));
		}
	}
}


void ImGui_Collider2D(Collider2D *collider)
{
	if (CollapsingHeader("Collider"))
	{
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
		Collider2D::colliderType index = collider->m_colliderType;
		Combo("Collider Type", reinterpret_cast<int *>(&index), collider_types, static_cast<int>(Collider2D::colliderType::collider_max));
		collider->m_colliderType = index;
	}
}

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
#include "graphics\Particles.h"
#include "Engine\Physics\Collider2D.h"
#include "Scripting\ScriptComponent.h"

#include "graphics\DebugGraphic.h"

using namespace ImGui;

#define GAMEOBJECT_WINDOW_SIZE ImVec2(375, 600)
#define GAMEOBJECT_WINDOW_POS  ImVec2(15, 20)

template <class Component, typename T>
void Action_General(EditorAction& a)
{
	ComponentHandle<Component> handle(a.handle);
	meta::Any obj(handle.Get());

	if (a.redo)
	{
		obj.SetPointerMember(a.name, a.new_value.GetData<T>());
	}
	else
	{
		obj.SetPointerMember(a.name, a.old_value.GetData<T>());
	}
}


template <>
void Action_General<SpriteComponent, ResourceID>(EditorAction& a)
{
	ComponentHandle<SpriteComponent> handle(a.handle);

	if (a.redo)
	{
		handle->SetResourceID(a.new_value.GetData<ResourceID>());
	}
	else
	{
		handle->SetResourceID(a.old_value.GetData<ResourceID>());
	}
}

enum ErrorIndex
{
	FailedToStartEditor = 1,
	HasComponent,
	HasRigidBodyDynamicCollider,
	HasStaticCollider
};

const char * ErrorList[] = 
{
	"Error 00: See Max if you see this",
	"Error 01: Cannot Start the Editor",
	"Error 02: Already has this Component Type.",
	"Error 03: This Object has a RigidBody or a Dynamic Collider.",
	"Error 04: This Object has a Static Collider.",
	"Error 04: ",
	"Error 05: ",
	"Error 06: ",
	"Error 07: ",
	"Error 08: ",
	"Error 09: ",
	"Error 10: "
};


#define HAS_COMPONENT editor->AddPopUp(PopUpWindow(ErrorList[HasComponent], 2.0f, PopUpPosition::Mouse))


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
					HAS_COMPONENT;
				}
				else
				{
					object.AddComponent<SpriteComponent>();
				}
			}
			else if (Button("Particle System"))
			{
				if (object.GetComponent<ParticleSystem>().IsValid())
				{
					HAS_COMPONENT;
				}
				else
				{
					object.AddComponent<ParticleSystem>();
				}
			}
			else if (Button("RigidBody"))
			{
				if (object.GetComponent<RigidBodyComponent>().IsValid())
				{
					HAS_COMPONENT;
				}
				else
				{
					object.AddComponent<RigidBodyComponent>();
					if (object.GetComponent<DynamicCollider2DComponent>().IsValid())
					{
					}
					else
					{
						object.AddComponent<DynamicCollider2DComponent>();
					}
				}
			}
			else if (Button("Dynamic Collider"))
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
						object.AddComponent<DynamicCollider2DComponent>();
						if (!object.GetComponent<RigidBodyComponent>().IsValid())
						{
							object.AddComponent<RigidBodyComponent>();
							editor->AddPopUp(PopUpWindow("Added a RigidBody Component.", 1.5f, PopUpPosition::Mouse));
						}
					}
				}
			}
			else if (Button("Static Collider"))
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
						object.AddComponent<StaticCollider2DComponent>(Collider2D::colliderType::colliderBox, glm::vec3(1, 1, 0));
					}
				}
			}
			else if (Button("Script"))
			{
				if (object.GetComponent<ScriptComponent>().IsValid())
				{
					HAS_COMPONENT;
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
	DebugGraphic::DrawShape(transform->GetPosition(), transform->GetScale(), 0.0f, glm::vec4(0,1,0,1));

	if (CollapsingHeader("Transform"))
	{
		EditorComponentHandle handle = { object.Getid(), true };
		
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
			glm::vec3 position = transform->m_position;
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
					editor->Push_Action({ position, transform->m_position, "position", handle, Action_General<TransformComponent, glm::vec3> });
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
			glm::vec3 scale = transform->m_scale;
			PushItemWidth(120);

			if (InputFloat("X##scale", &transform->m_scale.x, 0.0f, 0.0f, -1, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				editor->Push_Action({ scale, transform->m_scale, "scale", handle, Action_General<TransformComponent, glm::vec3> });
			}

			if (InputFloat("Y##scale", &transform->m_scale.y, 0.0f, 0.0f, -1, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				editor->Push_Action({ scale, transform->m_scale, "scale", handle, Action_General<TransformComponent, glm::vec3> });
			}
			
			PopItemWidth();
			TreePop();
			Separator();
		}

		DragFloat("Rotation##rotation_drag", &transform->m_rotation, 1, 0);
	}
}


void ImGui_RigidBody(RigidBodyComponent *rb, GameObject object, Editor * editor)
{
	if (CollapsingHeader("RigidBody"))
	{
		EditorComponentHandle handle = { object.Getid(), true };
		float mass = 1 / rb->Mass();

		if (Button("Remove##rigidbody"))
		{
			object.DeleteComponent<RigidBodyComponent>();

			if (object.GetComponent<DynamicCollider2DComponent>().IsValid())
			{
				object.AddComponent<StaticCollider2DComponent>(object.GetComponent<DynamicCollider2DComponent>().Get());
				object.DeleteComponent<DynamicCollider2DComponent>();
				editor->AddPopUp(PopUpWindow("Converted to Static Collider.", 1.5f, PopUpPosition::Mouse));
			}
			return;
		}

		if (TreeNode("Acceleration"))
		{
			glm::vec2 acc = rb->m_acceleration;
			PushItemWidth(120);
			
			if (InputFloat(" X##acceleration", &rb->m_acceleration.x))
			{
				editor->Push_Action({ acc, rb->m_acceleration, "acceleration", handle, Action_General<RigidBodyComponent, glm::vec3> });
			}

			if (InputFloat(" Y##acceleration", &rb->m_acceleration.y))
			{
				editor->Push_Action({ acc, rb->m_acceleration, "acceleration", handle, Action_General<RigidBodyComponent, glm::vec3> });
			}
			
			
			PopItemWidth();
			TreePop();
			Separator();
		}
		if (TreeNode("Velocity"))
		{
			glm::vec3 vel = rb->m_velocity;
			PushItemWidth(120);

			if (InputFloat("X##velocity", &rb->m_velocity.x))
			{
				editor->Push_Action({ vel, rb->m_velocity, "velocity", handle, Action_General<RigidBodyComponent, glm::vec3> });
			}

			if (InputFloat("Y##velocity", &rb->m_velocity.y))
			{
				editor->Push_Action({ vel, rb->m_velocity, "velocity", handle, Action_General<RigidBodyComponent, glm::vec3> });
			}
			
			PopItemWidth();
			TreePop();
			Separator();
		}
		if (TreeNode("Gravity"))
		{
			glm::vec3 gravity = rb->m_gravity;
			PushItemWidth(120);

			if (InputFloat("X##gravity", &rb->m_gravity.x))
			{
				editor->Push_Action({ gravity, rb->m_gravity, "gravity", handle, Action_General<RigidBodyComponent, glm::vec3> });
			}

			if (InputFloat("Y##gravity", &rb->m_gravity.y))
			{
				editor->Push_Action({ gravity, rb->m_gravity, "gravity", handle, Action_General<RigidBodyComponent, glm::vec3> });
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
		EditorComponentHandle handle = { object.Getid(), true };
		if (Button("Remove##sprite"))
		{
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

		std::string name = rm.Get(id)->FileName();

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


void ImGui_Collider2D(Collider2D *collider, GameObject object, Editor * editor)
{
	if (CollapsingHeader("Collider"))
	{
		EditorComponentHandle handle = { object.Getid(), true };
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
			PushItemWidth(120);

			glm::vec3 dim = collider->m_dimensions;

			if (InputFloat(" X##dim", &collider->m_dimensions.x))
			{
				if (collider->isStatic())
				{
					editor->Push_Action({ dim, collider->m_collisionLayer, "collisionLayer",
						handle, Action_General<StaticCollider2DComponent, int> });
				}
				else
				{
					editor->Push_Action({ dim, collider->m_collisionLayer, "collisionLayer",
						handle, Action_General<DynamicCollider2DComponent, int> });
				}
			}

			if (InputFloat(" Y##dim", &collider->m_dimensions.y))
			{
				if (collider->isStatic())
				{
					editor->Push_Action({ dim, collider->m_dimensions, "collisionLayer",
						handle, Action_General<StaticCollider2DComponent, int> });
				}
				else
				{
					editor->Push_Action({ dim, collider->m_dimensions, "collisionLayer",
						handle, Action_General<DynamicCollider2DComponent, glm::vec3> });
				}
			}

			PopItemWidth();
			TreePop();
			Separator();
		}
		if (TreeNode("Offset"))
		{
			PushItemWidth(120);
			glm::vec3 offset = collider->m_offset;
			if (InputFloat(" X##offset", &collider->m_offset.x))
			{
				if (collider->isStatic())
				{
					editor->Push_Action({ offset, collider->m_offset, "offset", handle, Action_General<StaticCollider2DComponent, glm::vec3> });
				}
				else
				{
					editor->Push_Action({ offset, collider->m_offset, "offset", handle, Action_General<DynamicCollider2DComponent, glm::vec3> });
				}
			}

			if (InputFloat(" Y##offset", &collider->m_offset.y))
			{

			}

			PopItemWidth();
			TreePop();
			Separator();
		}
		int index = collider->m_colliderType;
		for (int i = 0; i < sizeof(Collider2D::colliderType) * 8; i++)
		{
			if (index & (1 << i))
			{
				index = i - 1;
				break;
			}
		}

		SliderFloat("Elasticity", &collider->m_selfElasticity, 0.0f, 1.0f);
		

		// Collision Type
		Combo("Collider Type", &index, collider_types, static_cast<int>(Collider2D::colliderType::collider_max) - 2);
		switch (index)
		{
		case 0:
			collider->m_colliderType = Collider2D::colliderType::colliderBox;
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

			if (collider->isStatic())
			{
				editor->Push_Action({ layer, collider->m_collisionLayer, "collisionLayer", 
								handle, Action_General<StaticCollider2DComponent, int> });
			}
			else
			{
				editor->Push_Action({ layer, collider->m_collisionLayer, "collisionLayer", 
								handle, Action_General<DynamicCollider2DComponent, int> });
			}

			collider->m_collisionLayer = CollisionLayer(layer);

			TreePop();
		}
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


void ImGui_Particles(ParticleSystem *particles, GameObject object, Editor *editor)
{
	if (CollapsingHeader("Particle System"))
	{

		if (Button("Remove##particles"))
		{
			object.DeleteComponent<ParticleSystem>();
			return;
		}

		ParticleSettings& settings = particles->m_settings;
		
		// PushItemWidth(115);
		
		Checkbox("Looping", &settings.isLooping);
		SliderFloat("Rate", &settings.EmissionRate, SLIDER_STEP, 0);
		DragInt("Count", &settings.ParticlesPerEmission);
		
		if (TreeNode("Burst##particles"))
		{
			InputFloat("Frequency", &settings.BurstEmission.z, SLIDER_STEP, 0);

			DragFloat("Min Count", &settings.BurstEmission.x, SLIDER_STEP, 0);

			DragFloat("Max Count", &settings.BurstEmission.y, SLIDER_STEP, 0);

			Separator();
			TreePop();
		}

		

		Combo("Shape##particles", reinterpret_cast<int *>(&settings.EmissionShape), EmissionShape_Names, _countof(EmissionShape_Names));

		Separator();

		if (TreeNode("Shape Scale##particles"))
		{
			DragFloat("X##emission_shape", &settings.EmissionShapeScale.x, SLIDER_STEP, 0);

			DragFloat("Y##emission_shape", &settings.EmissionShapeScale.y, SLIDER_STEP, 0);

			TreePop();
		}

		Separator();

		Combo("Simulation Space##particles", reinterpret_cast<int *>(&settings.ParticleSpace), SimulationSpace_Names, _countof(SimulationSpace_Names));

		DragFloat("Emitter Lifetime", &settings.EmitterLifetime, SLIDER_STEP, 0);
		DragFloat("Lifetime", &settings.ParticleLifetime, SLIDER_STEP, 0);
		DragFloat("Lifetime Variance", &settings.ParticleLifetimeVariance, SLIDER_STEP, 0);

		Separator();

		if (TreeNode("Velocity##particles"))
		{
			DragFloat("X##init_velocity_particles", &settings.StartingVelocity.x, SLIDER_STEP, 0);
			DragFloat("Y##init_velocity_particles", &settings.StartingVelocity.y, SLIDER_STEP, 0);

			if (TreeNode("Variance##particles"))
			{
				DragFloat("X##Variance_velocity_particles", &settings.StartingVelocityVariance.x, SLIDER_STEP);
				DragFloat("Y##Variance_velocity_particles", &settings.StartingVelocityVariance.y, SLIDER_STEP);

				TreePop();
			}
			Separator();
			TreePop();
		}

		if (TreeNode("Acceleration##particles"))
		{
			DragFloat("X##Acceleration_particles", &settings.Acceleration.x, SLIDER_STEP);
			DragFloat("Y##Acceleration_particles", &settings.Acceleration.y, SLIDER_STEP);

			Separator();
			TreePop();
		}

		if (TreeNode("Scale Progression##particles"))
		{
			DragFloat2("Start Scale##particles", &settings.ScaleOverTime.x, SLIDER_STEP);
			DragFloat2("End Scale##particles", &settings.ScaleOverTime.z, SLIDER_STEP);

			Separator();
			TreePop();
		}

		if (TreeNode("Rotation##rotation_particles"))
		{
			DragFloat("Start Rotation##particles", &settings.StartRotation, SLIDER_STEP);
			DragFloat("Start Rotation Variance##particles", &settings.StartRotationVariation, SLIDER_STEP);
			DragFloat("Rotation Rate##particles", &settings.RotationRate, SLIDER_STEP);

			Separator();
			TreePop();
		}

		if (TreeNode("Sprite Data##particles"))
		{
			DragFloat4("Start Color##particles", &settings.StartColor.x, SLIDER_STEP);
			DragFloat4("End Color##particles", &settings.EndColor.x, SLIDER_STEP);

			if (TreeNode("Sprite"))
			{
				Separator();
				TreePop();
			}
			Separator();
			TreePop();
		}

		if (TreeNode("Trail##particles"))
		{
			Checkbox("Has Trail##particles", &settings.HasTrail);
			DragFloat("Rate##particles", &settings.TrailEmissionRate, SLIDER_STEP);
			DragFloat("Lifetime##particles", &settings.TrailLifetime, SLIDER_STEP);

			DragFloat4("Start Color##particles_trail", &settings.StartColor.x, SLIDER_STEP);
			DragFloat4("End Color##particles_trail", &settings.EndColor.x, SLIDER_STEP);

			Separator();
			TreePop();
		}
		// PopItemWidth();
	}
}




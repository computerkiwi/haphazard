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

#include "Input\Input.h"

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

bool dragClicked = false;

#define Drag_Key Key::A

#define Drag(NAME, SAVE, ITEM)																					 \
	if (DragFloat_ReturnOnClick(NAME, &ITEM, SLIDER_STEP))													 \
	{																										 \
		if (dragClicked == false)																			 \
		{																									 \
			SAVE = ITEM;																					 \
			dragClicked = true;																				 \
		}																									 \
	}																										 

#define Drag_Int(NAME, SAVE, ITEM)																					 \
	if (DragInt_ReturnOnClick(NAME, &ITEM, SLIDER_STEP))													 \
	{																										 \
		if (dragClicked == false)																			 \
		{																									 \
			SAVE = ITEM;																					 \
			dragClicked = true;																				 \
		}																									 \
	}

#define DragRelease(COMPONENT, SAVE, ITEM, META_NAME)														 \
	if (Input::IsReleased(Drag_Key) && dragClicked == true)																	 \
	{																										 \
		editor->Push_Action({ SAVE, ITEM,  META_NAME, handle, Action_General<COMPONENT, decltype(ITEM)> });  \
		dragClicked = false;																				 \
	}


#define DragRelease_Type(COMPONENT, SAVE, ITEM, META_NAME, TYPE)														 \
	if (Input::IsReleased(Key::A))																	 \
	{																										 \
		editor->Push_Action({ SAVE, ITEM,  META_NAME, handle, Action_General<COMPONENT, TYPE> });  \
		dragClicked = false;																				 \
	}

// Transform Component Save Location
TransformComponent transformSave;
RigidBodyComponent rigidBodySave;
Collider2D         colliderSave;
ParticleSettings   particleSave;


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

void ImGui_ObjectInfo(ObjectInfo *info, Editor *editor)
{
	if (info)
	{
		Separator();
		Text("ID: %d | %s", info->m_id & 0xFFFFFF, info->m_name.c_str());

		if (Button("Tags"))
		{
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
			for (auto& tag : info->m_tags)
			{
				if (Button("x##object_info_remove_tag"))
				{
					info->m_tags.erase(tag.first);
					break;
				}
				SameLine();
				Text(tag.second.c_str());
			}

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
			}

			EndPopup();
		}
	}
}

#define SLIDER_STEP 0.01f
#define ID_MASK 0xFFFFFF

void ImGui_Transform(TransformComponent *transform, GameObject object, Editor *editor)
{
	glm::vec2 scale(transform->GetScale());
	DebugGraphic::DrawShape(transform->GetPosition(), scale + glm::vec2(0.025f, 0.025f), transform->GetRotation(), glm::vec4(0,1,0,1));
	DebugGraphic::DrawShape(transform->GetPosition() + glm::vec2( scale.x / 2,  scale.y / 2), glm::vec2(0.025f, 0.025f), transform->GetRotation(), glm::vec4(0, 1, 0, 1));
	DebugGraphic::DrawShape(transform->GetPosition() + glm::vec2( scale.x / 2, -scale.y / 2), glm::vec2(0.025f, 0.025f), transform->GetRotation(), glm::vec4(0, 1, 0, 1));
	DebugGraphic::DrawShape(transform->GetPosition() + glm::vec2(-scale.x / 2, -scale.y / 2), glm::vec2(0.025f, 0.025f), transform->GetRotation(), glm::vec4(0, 1, 0, 1));
	DebugGraphic::DrawShape(transform->GetPosition() + glm::vec2(-scale.x / 2,  scale.y / 2), glm::vec2(0.025f, 0.025f), transform->GetRotation(), glm::vec4(0, 1, 0, 1));


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
				bool x_click = false;
				bool y_click = false;

				Text("X: %f", transform->GetPosition().x);
				Text("Y: %f", transform->GetPosition().y);

				// Position Widgets
				Drag("X Offset##transform", transformSave.m_position.x, transform->m_position.x);
				Drag("Y Offset##transform", transformSave.m_position.y, transform->m_position.y);

				DragRelease_Type(TransformComponent, transformSave.m_position, transform->m_position, "position", glm::vec2);
			}
			else
			{
				Drag("X##transform_position", transformSave.m_position.x, transform->m_position.x);
				Drag("Y##transform_position", transformSave.m_position.y, transform->m_position.y);

				DragRelease(TransformComponent, glm::vec2(transformSave.m_position), glm::vec2(transform->m_position), "position");
			}

			TreePop();
			Separator();
		}
		if (TreeNode("Scale"))
		{

			Drag("X##scale", transformSave.m_scale.x, transform->m_scale.x);
			Drag("Y##scale", transformSave.m_scale.y, transform->m_scale.y);

			DragRelease(TransformComponent, transformSave.m_scale, transform->m_scale, "scale");
			
			TreePop();
			Separator();
		}

		Drag("Rotation##transform", transform->m_rotation, transformSave.m_rotation);
		DragRelease(TransformComponent, transform->m_rotation, transformSave.m_rotation, "rotation");
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
			Drag("X##acceleration", rigidBodySave.m_acceleration.x, rb->m_acceleration.x);
			Drag("Y##acceleration", rigidBodySave.m_acceleration.y, rb->m_acceleration.y);

			DragRelease(RigidBodyComponent, rigidBodySave.m_acceleration, rigidBodySave.m_acceleration, "acceleration");
			
			TreePop();
			Separator();
		}
		if (TreeNode("Velocity"))
		{
			Drag("X##rigidbody_velocity", rigidBodySave.m_velocity.x, rb->m_velocity.x);
			Drag("Y##rigidbody_velocity", rigidBodySave.m_velocity.y, rb->m_velocity.y);

			DragRelease(RigidBodyComponent, rigidBodySave.m_velocity, rb->m_velocity, "velocity");

			TreePop();
			Separator();
		}
		if (TreeNode("Gravity"))
		{
			Drag("X##gravity", rigidBodySave.m_gravity.x, rb->m_gravity.x);
			Drag("Y##gravity", rigidBodySave.m_gravity.y, rb->m_gravity.y);

			DragRelease(RigidBodyComponent, rigidBodySave.m_gravity, rb->m_gravity, "gravity");
			
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
			Drag("X##collider_dim", colliderSave.m_dimensions.x, collider->m_dimensions.x);
			Drag("Y##collider_dim", colliderSave.m_dimensions.y, collider->m_dimensions.y);

			if (Input::IsReleased(Drag_Key))
			{						
				// Check if we need to save the action for static or dynamic
				if (collider->isStatic())
				{
					editor->Push_Action({ colliderSave.m_dimensions, collider->m_dimensions, "dimensions",
						handle, Action_General<StaticCollider2DComponent, int> });
				}
				else
				{
					editor->Push_Action({ colliderSave.m_dimensions, collider->m_dimensions, "dimensions",
						handle, Action_General<DynamicCollider2DComponent, int> });
				}  
				dragClicked = false;																				 
			}

			

			TreePop();
			Separator();
		}
		if (TreeNode("Offset"))
		{
			Drag("X##collider_offset", colliderSave.m_offset.x, collider->m_offset.x);
			Drag("Y##collider_offset", colliderSave.m_offset.y, collider->m_offset.y);


			if (Input::IsReleased(Drag_Key))
			{
				// Check if we need to save the action for static or dynamic
				if (collider->isStatic())
				{
					editor->Push_Action({ colliderSave.m_offset, collider->m_offset, "offset", handle, Action_General<StaticCollider2DComponent, glm::vec3> });
				}
				else
				{
					editor->Push_Action({ colliderSave.m_offset, collider->m_offset, "offset", handle, Action_General<DynamicCollider2DComponent, glm::vec3> });
				}
				dragClicked = false;
			}

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

			// Check if we need to save the action for static or dynamic
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
				script_c->scripts.emplace_back(LuaScript(engine->GetResourceManager().Get(buffer), object));
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
		EditorComponentHandle handle = { object.Getid(), true };

		if (Button("Remove##particles"))
		{
			object.DeleteComponent<ParticleSystem>();
			return;
		}

		ParticleSettings& settings = particles->m_settings;
		
		// PushItemWidth(115);
		
		Checkbox("Looping", &settings.isLooping);
		
		Drag("Rate##particles", particleSave.emissionRate, settings.emissionRate);
		DragRelease(ParticleSystem, particleSave.emissionRate, settings.emissionRate, "EmissionRate");

		DragInt("Count", &settings.particlesPerEmission, 0.25f);

		Drag_Int("Count##particles", particleSave.particlesPerEmission, settings.particlesPerEmission);
		if (settings.particlesPerEmission < 0)
		{
			settings.particlesPerEmission = 0;
		}
		DragRelease(ParticleSettings, particleSave.particlesPerEmission, settings.particlesPerEmission, "ParticlesPerEmission");


		if (TreeNode("Burst##particles"))
		{
			InputFloat("Frequency", &settings.burstEmission.z, SLIDER_STEP, 0);

			Drag("Min Count##particle", particleSave.burstEmission.x, settings.burstEmission.x);
			Drag("Max Count##particle", particleSave.burstEmission.y, settings.burstEmission.y);

			DragRelease(ParticleSettings, particleSave.burstEmission, settings.burstEmission, "BurstEmission");
			Separator();
			TreePop();
		}

		

		Combo("Shape##particles", reinterpret_cast<int *>(&settings.emissionShape), EmissionShape_Names, _countof(EmissionShape_Names));

		Separator();

		if (TreeNode("Shape Scale##particles"))
		{
			Drag("X##particle_emission_rate", particleSave.emissionShapeScale.x, settings.emissionShapeScale.x);
			Drag("Y##particle_emission_rate", particleSave.emissionShapeScale.y, settings.emissionShapeScale.y);

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
			Drag("X##particles_init_velocity", particleSave.startingVelocity.x, settings.startingVelocity.x);
			Drag("Y##particles_init_velocity", particleSave.startingVelocity.y, settings.startingVelocity.y);

			DragRelease(ParticleSettings, particleSave.startingVelocity, settings.startingVelocity, "StartingVelocity");

			if (TreeNode("Variance##particles"))
			{
				Drag("X##particles_variance_velocity", particleSave.startingVelocityVariance.x, settings.startingVelocityVariance.x);
				Drag("Y##particles_variance_velocity", particleSave.startingVelocityVariance.y, settings.startingVelocityVariance.y);

				DragRelease(ParticleSettings, particleSave.startingVelocityVariance, settings.startingVelocityVariance, "StartingVelocityVariance");

				TreePop();
			}
			Separator();
			TreePop();
		}

		if (TreeNode("Acceleration##particles"))
		{
			Drag("X##particles_acceleration", particleSave.acceleration.x, settings.acceleration.x);
			Drag("Y##particles_acceleration", particleSave.acceleration.y, settings.acceleration.y);

			DragRelease(ParticleSettings, particleSave.acceleration, settings.acceleration, "Acceleration");

			Separator();
			TreePop();
		}

		if (TreeNode("Scale Progression##particles"))
		{
			Text("Start");
			Drag("X##particles_scale_start", particleSave.scaleOverTime.x, settings.scaleOverTime.x);
			Drag("Y##particles_scale_start", particleSave.scaleOverTime.y, settings.scaleOverTime.y);

			Text("End");
			Drag("X##particles_scale_end", particleSave.scaleOverTime.z, settings.scaleOverTime.z);
			Drag("Y##particles_scale_end", particleSave.scaleOverTime.w, settings.scaleOverTime.w);

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
				Drag("R##particles_startColor", particleSave.startColor.x, settings.startColor.x);
				Drag("G##particles_startColor", particleSave.startColor.y, settings.startColor.y);
				Drag("B##particles_startColor", particleSave.startColor.z, settings.startColor.z);
				Drag("A##particles_startColor", particleSave.startColor.w, settings.startColor.w);

				DragRelease(ParticleSystem, particleSave.startColor, settings.startColor, "StartColor");

				Separator();

				Text("End Color");
				Drag("R##particles_endColor", particleSave.endColor.x, settings.endColor.x);
				Drag("G##particles_endColor", particleSave.endColor.y, settings.endColor.y);
				Drag("B##particles_endColor", particleSave.endColor.z, settings.endColor.z);
				Drag("A##particles_endColor", particleSave.endColor.w, settings.endColor.w);

				DragRelease(ParticleSystem, particleSave.endColor, settings.endColor, "EndColor");


				TreePop();
			}

			if (TreeNode("Sprite"))
			{
				// Add Texture * on the ParticleSettings struct
				Separator();
				TreePop();
			}
			Separator();
			TreePop();
		}

		if (TreeNode("Trail##particles"))
		{
			Checkbox("Has Trail##particles", &settings.hasTrail);
			Drag("Rate##particles_trail", particleSave.trailEmissionRate, settings.trailEmissionRate);
			DragRelease(ParticleSystem, particleSave.trailEmissionRate, settings.trailEmissionRate, "TrailEmissionRate");

			Drag("Lifetime##particles_trail", particleSave.trailLifetime, settings.trailLifetime);
			DragRelease(ParticleSystem, particleSave.trailLifetime, settings.trailLifetime, "TrailLifetime");

			if (TreeNode("Color##trail_particles"))
			{
				Text("Start Color");
				Drag("R##particles_trail_startColor", particleSave.trailStartColor.x, settings.trailStartColor.x);
				Drag("G##particles_trail_startColor", particleSave.trailStartColor.y, settings.trailStartColor.y);
				Drag("B##particles_trail_startColor", particleSave.trailStartColor.z, settings.trailStartColor.z);
				Drag("A##particles_trail_startColor", particleSave.trailStartColor.w, settings.trailStartColor.w);
				
				DragRelease(ParticleSystem, particleSave.trailStartColor, settings.trailStartColor, "TrailStartColor");

				Separator();

				Text("End Color");
				Drag("R##particles_trail_endColor", particleSave.trailEndColor.x, settings.trailEndColor.x);
				Drag("G##particles_trail_endColor", particleSave.trailEndColor.y, settings.trailEndColor.y);
				Drag("B##particles_trail_endColor", particleSave.trailEndColor.z, settings.trailEndColor.z);
				Drag("A##particles_trail_endColor", particleSave.trailEndColor.w, settings.trailEndColor.w);

				DragRelease(ParticleSystem, particleSave.trailEndColor, settings.trailEndColor, "TrailEndColor");

				TreePop();
			}

			Separator();
			TreePop();
		}
		// PopItemWidth();
	}
}




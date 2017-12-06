/*
FILE: Type_Binds.h
PRIMARY AUTHOR: Sweet

Copyright � 2017 DigiPen (USA) Corporation.
*/

#pragma once

class Editor;
class GameObject;
typedef int GameObject_ID;
class ObjectInfo;
class TransformComponent;
class RigidBodyComponent;
class SpriteComponent;
class Collider2D;
class ParticleSystem;
struct ScriptComponent;

class Camera;
class BackgroundComponent;

class TextComponent;

enum ErrorIndex
{
	FailedToStartEditor = 1,
	HasComponent,
	HasRigidBodyDynamicCollider,
	HasStaticCollider,
	OpenFileError
};


#define MAX_SELECT 10

void LoadPreFab(Editor *editor);
void SavePrefab(GameObject object);

void ImGui_GameObject(GameObject object, Editor *editor);
void ImGui_GameObject_Multi(Array<GameObject_ID, MAX_SELECT>& objects, Editor *editor);

void ImGui_ObjectInfo(ObjectInfo *info, Editor *editor);
void ImGui_Transform(TransformComponent *transform, GameObject object, Editor *editor);
void ImGui_RigidBody(RigidBodyComponent *rb, GameObject object, Editor * editor);
void ImGui_Sprite(SpriteComponent *sprite, GameObject object, Editor * editor);
void ImGui_Collider2D(Collider2D *collider, GameObject object, Editor * editor);
void ImGui_Script(ScriptComponent *script_c, GameObject object, Editor * editor);
void ImGui_Particles(ParticleSystem *particles, GameObject object, Editor *editor);
void ImGui_Camera(Camera *camera, GameObject object, Editor *editor);
void ImGui_Background(BackgroundComponent *background, GameObject object, Editor *editor);
void ImGui_Text(TextComponent *text, GameObject object, Editor *editor);


// Error Message for when adding a component that exist
#define HAS_COMPONENT editor->AddPopUp(PopUpWindow(ErrorList[HasComponent], 2.0f, PopUpPosition::Mouse))

// Error Message for when adding a component that exist
#define HAS_COMPONENT_Editor AddPopUp(PopUpWindow(ErrorList[HasComponent], 2.0f, PopUpPosition::Mouse))

// Pushes the add component to the undo/redo system
#define Push_AddComponent(TYPE) editor->Push_Action({ 0, 0, nullptr, { object.Getid(), true }, Action_AddComponent<TYPE> })


#define Push_AddComponent_Editor(TYPE) Push_Action({ 0, 0, nullptr, { m_selected_object.Getid(), true }, Action_AddComponent<TYPE> })


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

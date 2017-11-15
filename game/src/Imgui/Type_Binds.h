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


enum ErrorIndex
{
	FailedToStartEditor = 1,
	HasComponent,
	HasRigidBodyDynamicCollider,
	HasStaticCollider,
	OpenFileError
};


#define MAX_SELECT 10

void ImGui_GameObject(GameObject object, Editor *editor);
void ImGui_GameObject_Multi(Array<GameObject_ID, MAX_SELECT>& objects, Editor *editor);

void ImGui_ObjectInfo(ObjectInfo *info, Editor *editor);
void ImGui_Transform(TransformComponent *transform, GameObject object, Editor *editor);
void ImGui_RigidBody(RigidBodyComponent *rb, GameObject object, Editor * editor);
void ImGui_Sprite(SpriteComponent *sprite, GameObject object, Editor * editor);
void ImGui_Collider2D(Collider2D *collider, GameObject object, Editor * editor);
void ImGui_Script(ScriptComponent *script_c, GameObject object, Editor * editor);
void ImGui_Particles(ParticleSystem *particles, GameObject object, Editor *editor);
void ImGui_Camera(Camera *camera, Editor *editor);


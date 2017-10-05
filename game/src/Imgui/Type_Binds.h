/*
FILE: Type_Binds.h
PRIMARY AUTHOR: Sweet

Copyright © 2017 DigiPen (USA) Corporation.
*/

#pragma once

class GameObject;
class TransformComponent;
class RigidBodyComponent;
class SpriteComponent;
class Collider2D;
struct ScriptComponent;

void ImGui_GameObject(GameObject *object);
void ImGui_Transform(TransformComponent *transform, GameObject *object);
void ImGui_RigidBody(RigidBodyComponent *rb, GameObject *object);
void ImGui_Sprite(SpriteComponent *sprite, GameObject *object);
void ImGui_Collider2D(Collider2D *collider, GameObject *object);
void ImGui_Script(ScriptComponent *script_c, GameObject *object);

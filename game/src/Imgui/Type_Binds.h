/*
FILE: Type_Binds.h
PRIMARY AUTHOR: Sweet

Copyright © 2017 DigiPen (USA) Corporation.
*/

#pragma once

class GameObject;
typedef int GameObject_ID;
class ObjectInfo;
class TransformComponent;
class RigidBodyComponent;
class SpriteComponent;
class Collider2D;

void ImGui_GameObject(GameObject object);
void ImGui_ObjectInfo(ObjectInfo *info);
void ImGui_Transform(TransformComponent *transform, GameObject object);
void ImGui_RigidBody(RigidBodyComponent *rb, GameObject object);
void ImGui_Sprite(SpriteComponent *sprite, GameObject object);
void ImGui_Collider2D(Collider2D *collider, GameObject object);

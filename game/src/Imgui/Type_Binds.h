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

void ImGui_GameObject(GameObject *object);
void ImGui_Transform(TransformComponent *transform);
void ImGui_RigidBody(RigidBodyComponent *rb);
void ImGui_Sprite(SpriteComponent *sprite);
void ImGui_Collider2D(Collider2D *collider);

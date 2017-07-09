/*
FILE: Collider2D.cpp
PRIMARY AUTHOR: Sweet

Collider Component Data Functions

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Collider2D.h"


Collider2D::Collider2D(GameObject * parent) : Collider2D(parent, CollisionLayers::NONE, true)
{
}



Collider2D::Collider2D(GameObject * parent, CollisionLayer_t layers) : Collider2D(parent, layers, true)
{
}


Collider2D::Collider2D(GameObject * parent, bool collision) : Collider2D(parent, CollisionLayers::NONE, collision)
{
}



Collider2D::Collider2D(GameObject * parent, CollisionLayer_t layers, bool collision) : Component(parent), mCollision(collision), mLayers(layers)
{	
}



CollisionLayer_t Collider2D::GetLayers() const
{
	assert(mParent && "Component does not have a parent!!");
	return mLayers;
}


void Collider2D::SetCollisionLayer(CollisionLayer_t layers)
{
	assert(mParent && "Component does not have a parent!!");
	mLayers = layers;
}


void Collider2D::AddLayer(CollisionLayers layer)
{
	assert(mParent && "Component does not have a parent!!");
	mLayers |= layer;
}


void Collider2D::RemoveLayer(CollisionLayers layer)
{
	assert(mParent && "Component does not have a parent!!");
	mLayers &= ~layer;
}


bool Collider2D::HasCollision() const
{
	assert(mParent && "Component does not have a parent!!");
	return mCollision;
}


void Collider2D::SetCollision(bool collision)
{
	assert(mParent && "Component does not have a parent!!");
	mCollision = collision;
}



void Collider2D::ToggleCollision()
{
	assert(mParent && "Component does not have a parent!!");
	mCollision = !mCollision;
}






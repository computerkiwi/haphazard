/*
FILE: Collider2D.cpp
PRIMARY AUTHOR: Sweet

Collider Component Data Functions

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Collider2D.h"



Collider2D::Collider2D(CollisionLayer_t layers) : Collider2D(layers, true)
{
}


Collider2D::Collider2D(bool collision) : Collider2D(CollisionLayers::NONE, collision)
{
}



Collider2D::Collider2D(CollisionLayer_t layers, bool collision) : mCollision(collision), mLayers(layers)
{	
}



CollisionLayer_t Collider2D::GetLayers() const
{
	return mLayers;
}


void Collider2D::SetCollisionLayer(CollisionLayer_t layers)
{
	mLayers = layers;
}


void Collider2D::AddLayer(CollisionLayers layer)
{
	mLayers |= layer;
}


void Collider2D::RemoveLayer(CollisionLayers layer)
{
	mLayers &= ~layer;
}


bool Collider2D::HasCollision() const
{
	return mCollision;
}


void Collider2D::SetCollision(bool collision)
{
	mCollision = collision;
}



void Collider2D::ToggleCollision()
{
	mCollision = !mCollision;
}






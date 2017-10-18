/*
FILE: CollisionLayer.h
PRIMARY AUTHOR: Brett Schiff

Collisionlayer definitions and interactions

Copyright � 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "meta/meta.h"

enum /*class */collisionLayers
{
	noCollision = 1 << 0,  // Collides with nothing
	allCollision = 1 << 1, // Collides with everything
	player = 1 << 2,
	ground = 1 << 3,
	enemy = 1 << 4,
	allyProjectile = 1 << 5,  // doesn't collide with other players
	decor = 1 << 6,

	numLayers = 7
};

class CollisionLayer
{
public:
	// conversion constructors
	explicit CollisionLayer(int layer = noCollision);
	CollisionLayer(const CollisionLayer& layer);

	// if both layers should collide with each other, return true, otherwise false
	bool LayersCollide(const CollisionLayer& other) const;

	// checks if two collisionLayers are IDENTICAL. Will return false if layers collide but are not the same
	bool operator==(CollisionLayer& layer);

private:
	int m_layer;

	META_REGISTER(CollisionLayer)
	{
		META_DefineType(CollisionLayer);
		META_DefineMember(CollisionLayer, m_layer, "layer");
	}
};
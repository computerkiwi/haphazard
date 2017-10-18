/*
FILE: CollisionLayer.cpp
PRIMARY AUTHOR: Brett Schiff

Collisionlayer definitions and interactions

Copyright © 2017 DigiPen (USA) Corporation.
*/
#include "CollisionLayer.h"

/*
enum collisionLayers
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
*/

int CollisionMasks[collisionLayers::numLayers] =
{ // In order: decor allyProjectile enemy ground player allCollision noCollision
	0b0000010,	// noCollision
	0b1111111,	// allCollision
	0b0011110,	// player     
	0b1111110,	// ground     
	0b0111110,	// enemy
	0b0011010,	// allyProjectile
	0b0001010	// decor
};


// conversion constructor
CollisionLayer::CollisionLayer(int layers) : m_layer(layers)
{
}

CollisionLayer::CollisionLayer(const CollisionLayer& layer) : m_layer(layer.m_layer)
{
}

// if both layers should collide with each other, return true, otherwise false
bool CollisionLayer::LayersCollide(const CollisionLayer& other) const
{
	// to extract the necessar index into CollisionMasks
	int thisIndex = 0;
	int otherIndex = 0;
	int thisLayer = m_layer;
	int otherLayer = other.m_layer;

	// extract the indices of the collision layer into the mask array
	while (thisLayer > 1)
	{
		thisLayer >>= 1;
		++thisIndex;
	}

	while (otherLayer > 1)
	{
		otherLayer >>= 1;
		++otherIndex;
	}

	// make sure this object's layers collides with the other object's layer
	int thisShouldCollide = m_layer & CollisionMasks[otherIndex];
	// make sure the other object's layer collides with this object's layer
	int otherShouldCollide = other.m_layer & CollisionMasks[thisIndex];

	// if both layers should collide with each other, return true, otherwise false
	return thisShouldCollide && otherShouldCollide;
}

// checks if two sets of collisionLayers are IDENTICAL. Will return false if one layer matches but another does not
bool CollisionLayer::operator==(CollisionLayer& layer)
{
	return m_layer == layer.m_layer;
}
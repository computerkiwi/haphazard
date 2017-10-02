/*
FILE: CollisionLayer.cpp
PRIMARY AUTHOR: Brett Schiff

Collisionlayer definitions and interactions

Copyright © 2017 DigiPen (USA) Corporation.
*/
#include "CollisionLayer.h"

enum collisionLayers
{
	noCollision = 1 << 0,  // Collides with nothing, even if the object is also on other layers
	player = 1 << 1,
	ground = 1 << 2,
	enemy = 1 << 3,

	numLayers = 4
};

enum class resolutionType
{
	noCollision = 0,
	stop = 1,
	bounce = 2
};

int CollisionInteraction[numLayers][numLayers];


// conversion constructor
CollisionLayer::CollisionLayer(int layers)
{

}

// checks if any layers collide, returning the layer on the second object onto which the first collided
resolutionType CollisionLayer::anyLayersCollide(int layers1, int layers2) const
{

}

// checks if the set of layers contains a specific layer
bool CollisionLayer::isOnLayer(collisionLayers layer) const
{

}

// checks if two sets of collisionLayers are IDENTICAL. Will return false if one layer matches but another does not
bool CollisionLayer::operator==(int layers)
{

}

// adds a layer to the set of layers, does nothing if set already contains the specified layer
void CollisionLayer::operator+=(collisionLayers layer)
{

}
// removes a layer from the set of layers, does nothing if set already doesn't contain the specified layer
void CollisionLayer::operator-=(collisionLayers layer)
{

}

// returns a set of layers where every layer in the passed set has been removed from the original
int CollisionLayer::operator-(int layers) const
{

}

// returns a set of layers where every layer in the passed set has been removed from the original
int CollisionLayer::operator+(int layers) const
{

}
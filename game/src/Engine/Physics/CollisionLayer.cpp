/*
FILE: CollisionLayer.cpp
PRIMARY AUTHOR: Brett Schiff

Collisionlayer definitions and interactions

Copyright © 2017 DigiPen (USA) Corporation.
*/
#include "CollisionLayer.h"
#include "../../Input/Input.h"

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
{ // In order: collectable, deadGnome, player4, player3, player2, player1, decor allyProjectile enemy ground player allCollision noCollision
	0b0000000000010,	// noCollision
	0b1111111111111,	// allCollision
	0b1111110011110,	// player     
	0b1111111111110,	// ground     
	0b0011110111110,	// enemy
	0b0100000011010,	// allyProjectile
	0b0000000001010,	// decor
	0b1011100011110,	// player1
	0b1011010011110,	// player2
	0b1010110011110,	// player3
	0b1001110011110,	// player4
	0b0000000101010,	// deadGnome
	0b0011110000110,	// collectable
};

// set two layers to collide with each other
void CollisionLayer_SetLayersColliding(int layer1, int layer2)
{
	int index1 = 0;
	int index2 = 0;

	int shiftedLayer1 = layer1;
	int shiftedLayer2 = layer2;

	// get the layermask indices
	while (shiftedLayer1 > 1)
	{
		++index1;
		shiftedLayer1 >>= 1;
	}
	while (shiftedLayer2 > 1)
	{
		++index2;
		shiftedLayer2 >>= 1;
	}

	CollisionMasks[index1] |= layer2;
	CollisionMasks[index2] |= layer1;
}

// set two layers not to collide with each other
void CollisionLayer_SetLayersNotColliding(int layer1, int layer2)
{
	int index1 = 0;
	int index2 = 0;

	int shiftedLayer1 = layer1;
	int shiftedLayer2 = layer2;

	// get the layermask indices
	while (shiftedLayer1 > 1)
	{
		++index1;
		shiftedLayer1 >>= 1;
	}
	while (shiftedLayer2 > 1)
	{
		++index2;
		shiftedLayer2 >>= 1;
	}

	CollisionMasks[index1] &= ~layer2;
	CollisionMasks[index2] &= ~layer1;
}

// this will make the layer collide ONLY with the layers passed and nothing else. (Or( | ) the layers together in the second parameter)
void CollisionLayer_SetCollidingLayers(collisionLayers layerToSet, int layersToCollideWith)
{
	// a mask to shift and depackage the passed layers
	int layerDepackageMask = 1;

	// get the collision mask index
	int index = 0;
	int shiftedLayerToSet = layerToSet;
	while (shiftedLayerToSet > 1)
	{
		++index;
		shiftedLayerToSet >>= 1;
	}

	// clear the layermask
	CollisionMasks[index] = 0;

	// depackage and set collison between each passed layer
	while (layerDepackageMask)
	{
		// will be 0 if the layer was specified, otherise it will be that layer
		int addedLayer = layerDepackageMask & layersToCollideWith;

		if (addedLayer)
		{
			CollisionMasks[index] |= addedLayer;
		}

		layerDepackageMask <<= 1;
	}
}

void CollisionLayerTestFuction(void)
{
	int groundMask = CollisionMasks[3];

	if (Input::IsPressed(Key::I))
	{
		CollisionLayer_SetCollidingLayers(collisionLayers::ground, collisionLayers::allCollision | collisionLayers::decor | collisionLayers::enemy);
		groundMask = CollisionMasks[3];
	}

	if (Input::IsPressed(Key::O))
	{
		CollisionLayer_SetLayersColliding(collisionLayers::ground, collisionLayers::player);
		groundMask = CollisionMasks[3];
	}
	
	if (Input::IsPressed(Key::P))
	{
		CollisionLayer_SetLayersNotColliding(collisionLayers::ground, collisionLayers::enemy);
		groundMask = CollisionMasks[3];
	}

	return;
}

// ********** CollisionLayer Class ********** //
// conversion constructor
CollisionLayer::CollisionLayer(int layers) : m_layer(layers)
{
}

CollisionLayer::CollisionLayer(collisionLayers layer) : m_layer(layer)
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

CollisionLayer::operator collisionLayers()
{
	return static_cast<collisionLayers>(m_layer);
}

CollisionLayer::operator int()
{
	return m_layer;
}

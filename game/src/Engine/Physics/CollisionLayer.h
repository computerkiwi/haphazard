/*
FILE: CollisionLayer.h
PRIMARY AUTHOR: Brett Schiff

Collisionlayer definitions and interactions

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

enum collisionLayers;

enum class resolutionType;

class CollisionLayer
{
public:
	// conversion constructor
	explicit CollisionLayer(int layers);

	// checks if any layers collide, returning the type of collision resolution between the two
	resolutionType anyLayersCollide(int layers1, int layers2) const;

	// checks if the set of layers contains a specific layer
	bool isOnLayer(collisionLayers layer) const;

	// checks if two sets of collisionLayers are IDENTICAL. Will return false if one layer matches but another does not
	bool operator==(int layers);

	// adds a layer to the set of layers, does nothing if set already contains the specified layer
	void operator+=(collisionLayers layer);
	// removes a layer from the set of layers, does nothing if set already doesn't contain the specified layer
	void operator-=(collisionLayers layer);

	// returns a set of layers where every layer in the passed set has been removed from the original
	int operator-(int layers) const;

	// returns a set of layers where every layer in the passed set has been removed from the original
	int operator+(int layers) const;

private:
	int m_layers;
};
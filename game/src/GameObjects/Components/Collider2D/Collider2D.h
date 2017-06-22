/*
FILE: Collider2D.h
PRIMARY AUTHOR: Sweet

Collider Component Definition

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "../Component.h"
#include <cstdint>

typedef uint32_t CollisionLayer_t;

enum CollisionLayers : CollisionLayer_t
{
	NONE = 0,
	World = 1 << 0,
	Trees = 1 << 1,
	Memes = 1 << 2,
	Flying = 1 << 3,
};


class Collider2D : public Component
{
public:

	explicit Collider2D(GameObject * parent);

	Collider2D(GameObject * parent, CollisionLayer_t layers, bool collision);

	explicit Collider2D(GameObject * parent, CollisionLayer_t layers);
	explicit Collider2D(GameObject * parent, bool collision);


	void SetCollisionLayer(CollisionLayer_t layers);
	inline void AddLayer(CollisionLayers layer);
	inline void RemoveLayer(CollisionLayers layer);

	CollisionLayer_t GetLayers() const;


	void ToggleCollision();
	void SetCollision(bool collision);
	bool HasCollision() const;

private:
	bool mCollision = true;

	CollisionLayer_t mLayers = CollisionLayers::NONE;
};

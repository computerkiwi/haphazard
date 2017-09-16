/*
FILE: PhysicsSystem.h
PRIMARY AUTHOR: Brett Schiff

Handles Movement, Colliders, Collision Detection, and Collision Resolution

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "../../GameObjectSystem/GameSpace.h"

class PhysicsSystem : public SystemBase
{
	virtual void Init();

	// Called each frame.
	virtual void Update(float dt);

	// Simply returns the default priority for this system.
	virtual size_t DefaultPriority()
	{
		return 1;
	}
};

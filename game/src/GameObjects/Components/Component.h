/*
FILE: Component.h
PRIMARY AUTHOR: Sweet

Entry point into the program.
Currently just a place to play around in.

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#if NO_COMPONENT_ASSERT
	// Used to Assert when parent is nullptr
	#define assert(exp)
#else
	#include <assert.h>
#endif


class GameObject;

class Component
{
public:
	explicit Component(GameObject * parent) : mParent(parent) {}

	inline void SetParent(GameObject * parent) { mParent = parent; }
	inline GameObject * GetParent() const { return mParent; }

protected:
	GameObject * mParent;
};



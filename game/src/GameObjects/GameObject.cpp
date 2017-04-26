/*
FILE: GameObject.cpp
PRIMARY AUTHOR: Sweet

Entry point into the program.
Currently just a place to play around in.

Copyright © 2017 DigiPen (USA) Corporation.
*/

#include "GameObject.h"

// ----------------------------------------------------
// 
// --------
GameObjectID_t GameObject::id() const
{
	return mID;
}


void GameObject::SetSpace(GameObject_Space * space)
{
	mContainingSpace = space;
}


GameObject_Space * GameObject::GetSpace() const
{
	return mContainingSpace;
}






/*
FILE: GameObject.cpp
PRIMARY AUTHOR: Sweet

Entry point into the program.
Currently just a place to play around in.

Copyright © 2017 DigiPen (USA) Corporation.
*/

#include "GameObject.h"


GameObject::GameObject(GameObject_Space & space) : mContainingSpace(space), mID(space.AssignID())
{
}



// ----------------------------------------------------
// 
// --------
GameObjectID_t GameObject::id() const
{
	return mID;
} 


GameObject_Space & GameObject::GetSpace() const
{
	return mContainingSpace;
}






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


GameObject::GameObject(GameObject_Space * space) : mContainingSpace(*space), mID(space->AssignID())
{
}


// ----------------------------------------------------
// This gets the id of a GameObject
// --------
GameObjectID_t GameObject::id() const
{
	return mID;
} 


GameObject_Space & GameObject::GetSpace() const
{
	return mContainingSpace;
}




std::ostream & operator<<(std::ostream & os, GameObject & object)
{
	os << "<GameObject " << "id:" << object.mID << ">";
	return os;
}



/*
FILE: Components.cpp
PRIMARY AUTHOR: Sweet

Entry point into the program.
Currently just a place to play around in.

Copyright © 2017 DigiPen (USA) Corporation.
*/

#include "../GameObject.h"
#include "Component.h"

Component::Component(GameObject * parent) : mParent(parent)
{
}


GameObject * Component::GetParent() const
{
	return mParent;
}




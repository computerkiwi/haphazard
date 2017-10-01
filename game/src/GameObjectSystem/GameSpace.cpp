/*
FILE: GameSpace.h
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/

#include "GameSpace.h"

static ComponentType lastID = 0;

ComponentType GenerateComponentTypeID()
{
	return lastID++;
}


GameObject_ID GenerateID()
{
	static GameObject_ID lastGeneratedID = 1;

	return lastGeneratedID++;
}

/*
FILE: Component.h
PRIMARY AUTHOR: Kieran

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "GameObject.h"

// Forward declaration.
class GameSpace;

template <typename T>
class ComponentHandle
{
public:
	ComponentHandle(GameObject_ID id, GameSpace *gameSpace) : m_objID(id), m_gameSpace(gameSpace)
	{
	}

	T *operator->()
	{
		return m_gameSpace->getInternalComponent(m_objID);
	}

	T& operator*()
	{
		return *m_gameSpace->getInternalComponent(m_objID);
	}

private:
	GameObject_ID m_objID;
	GameSpace *m_gameSpace;
};

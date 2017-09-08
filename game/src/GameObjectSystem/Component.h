/*
FILE: Component.h
PRIMARY AUTHOR: Kieran

Copyright � 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "GameObject.h"

// Forward declaration.
class GameSpace;

template <typename T>
class ComponentHandle
{
public:
	ComponentHandle(GameObject_ID id, GameSpace *gameSpace, bool isValid = true) : m_objID(id), m_gameSpace(gameSpace), m_isValid(isValid)
	{
	}

	T *operator->()
	{
		return m_gameSpace->getInternalComponent<T>(m_objID);
	}

	T& operator*()
	{
		return *m_gameSpace->getInternalComponent(m_objID);
	}

	GameObject GetGameObject()
	{
		return GameObject(m_objID, m_gameSpace);
	}

	// Returns true if this is a valid component.
	bool IsValid()
	{
		return m_isValid;
	}

private:
	GameObject_ID m_objID;
	GameSpace *m_gameSpace;
	bool m_isValid;
};

/*
FILE: Component.h
PRIMARY AUTHOR: Kieran

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "GameObjectSystem\GameObject.h"

// Forward declaration.
class GameSpace;
#define EXTRACTION_SHIFT (8 * 3)

template <typename T>
class ComponentHandle
{
public:
	ComponentHandle(GameObject_ID id, bool isValid = true) : m_objID(id), m_isValid(isValid)
	{
	}

	bool operator== (const ComponentHandle& other)
	{
		return m_objID == other.m_objID;
	}

	bool operator!=(const ComponentHandle& other)
	{
		return !(*this == other);
	}

	T *operator->()
	{
		return GameObject(m_objID).GetSpace()->GetInternalComponent<T>(GameObject(m_objID).Getid());
	}

	T *Get()
	{
		return GameObject(m_objID).GetSpace() ? operator->() : nullptr;
	}

	T& operator*()
	{
		return *m_gameSpace->GetInternalComponent<T>(m_objID);
	}

	GameObject GetGameObject() const
	{
		return m_objID;
	}


	GameObject_ID GetGameObject_ID() const
	{
		return m_objID;
	}


	template <typename T>
	ComponentHandle<T> GetSiblingComponent()
	{
		// Make sure the component exists before we hand it off.
		if (GameObject(m_objID).GetSpace()->GetInternalComponent<T>(GameObject(m_objID).Getid()) != nullptr)
		{
			return ComponentHandle<T>(m_objID);
		}
		else
		{
			return ComponentHandle<T>(0, false);
		}
	}

	// Returns true if this is a valid component.
	bool IsValid() const
	{
		return m_isValid;
	}

private:
	GameObject_ID m_objID;
	bool m_isValid;
};

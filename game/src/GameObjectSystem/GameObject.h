/*
FILE: GameObject.h
PRIMARY AUTHOR: Kieran

Copyright � 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Engine/Engine.h"

class GameSpace;

// GameObject ID Gen
GameObject_ID GenerateID();

#define EXTRACTION_SHIFT (8 * 7)
#define implicit

template <typename T>
class ComponentHandle;
class Engine;
extern Engine engine;

typedef int GameObject_ID;
typedef std::size_t GameSpaceIndex;
typedef int    dummy;

class GameObject
{
public:
	GameObject(GameObject_ID id, GameSpaceIndex gameSpace) : m_objID(id & (gameSpace << EXTRACTION_SHIFT))
	{
	}

	implicit GameObject(GameObject_ID id) : m_objID(id)
	{
	}

	template <typename T, typename... Args>
	void AddComponent(Args&&... args)
	{
		GetSpace()->EmplaceComponent<T>(m_objID, std::forward<Args>(args)...);
	}

	template <typename T>
	ComponentHandle<T> GetComponent()
	{
		// Make sure the component exists before we hand it off.
		if (GetSpace()->GetInternalComponent<T>(m_objID) != nullptr)
		{
			// Why the hell does constructing a ComponentHandle work?
			// We never forward declare the constructor and we shouldn't know what sizeof(ComponentHandle<T>) is, right? -Kieran
			return ComponentHandle<T>(m_objID, GetSpace());
		}
		else
		{
			return ComponentHandle<T>(0, nullptr, false);
		}
	}

	GameObject_ID Getid() const
	{
		return m_objID;
	}

	// Dummy Template param since GameSpace are forward delcared here
	template <typename dummy>
	GameObject_ID Duplicate() const
	{
		return GetSpace()->Duplicate(m_objID, GetSpace()->NewGameObject());
	}

	// Templated to avoid errors
	template <typename dummy>
	void Delete()
	{
		GetSpace()->Delete(m_objID);
		m_objID = 0;
	}

	GameSpace *GetSpace() const
	{
		// (0xFF00000000000000 & m_objID) >> EXTRACTION_SHIFT
		return engine.GetSpace(m_index);
	}

	GameSpaceIndex GetIndex() const
	{
		return m_index; // m_objID & 0xFF00000000000000;
	}

	// Templated to avoid errors
	template <typename AVOID>
	void SetSpace(GameSpaceIndex index)
	{
		m_index = index;
		// (m_objID &= 0x00FFFFFFFFFFFFFF) &= (index << EXTRACTION_SHIFT);
	}

	template <typename T>
	void DeleteComponent()
	{
		GetSpace()->DeleteComponent<T>(m_objID);
	}

	operator bool() const { return m_objID; }

private:
	union
	{
		struct
		{
			int m_id     : 24;
			int m_index  : 8;
		};
		GameObject_ID m_objID;
	};
};

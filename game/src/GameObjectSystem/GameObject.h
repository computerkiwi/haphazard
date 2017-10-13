/*
FILE: GameObject.h
PRIMARY AUTHOR: Kieran

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "meta/meta.h"

class GameSpace;

extern GameSpace *defaultGameSpacePtr;

template <typename T>
class ComponentHandle;

typedef size_t GameObject_ID;
typedef int    dummy;

class GameObject
{
public:
	GameObject(GameObject_ID id, GameSpace *gameSpace) : m_objID(id), m_gameSpace(gameSpace)
	{
	}

	// This is temporary for current serialization issues.
	// TODO: delet this
	GameObject() : m_objID(GameObject::GenerateID()), m_gameSpace(defaultGameSpacePtr)
	{
	}

	template <typename T, typename... Args>
	void AddComponent(Args&&... args)
	{
		m_gameSpace->EmplaceComponent<T>(m_objID, std::forward<Args>(args)...);
	}

	template <typename T>
	ComponentHandle<T> GetComponent()
	{
		// Make sure the component exists before we hand it off.
		if (m_gameSpace->GetInternalComponent<T>(m_objID) != nullptr)
		{
			// Why the hell does constructing a ComponentHandle work?
			// We never forward declare the constructor and we shouldn't know what sizeof(ComponentHandle<T>) is, right? -Kieran
			return ComponentHandle<T>(m_objID, m_gameSpace);
		}
		else
		{
			return ComponentHandle<T>(0, nullptr, false);
		}
	}

	static GameObject_ID GenerateID();

	static void SetHighestID(GameObject_ID highestID);

	GameObject_ID Getid() const
	{
		return m_objID;
	}

	// Dummy Template param since GameSpace are forward delcared here
	template <typename dummy>
	GameObject Duplicate()
	{
		return m_gameSpace->Duplicate(m_objID, m_gameSpace->NewGameObject().m_objID);
	}

	// Templated to avoid errors
	template <typename dummy>
	void Delete()
	{
		m_gameSpace->Delete(m_objID);
	}

	const GameSpace *GetSpace() const
	{
		return m_gameSpace;
	}

	// Templated to avoid errors
	template <typename AVOID>
	void SetSpace(GameSpace *space)
	{
		m_gameSpace = space;
	}

	template <typename T>
	void DeleteComponent()
	{
		m_gameSpace->DeleteComponent<T>(m_objID);
	}

	// These are pointers, not handles. They probably won't last long.
	std::vector<meta::Any> GetComponentPointersMeta();

private:
	GameObject_ID m_objID;
	GameSpace *m_gameSpace;

	static GameObject_ID lastGeneratedID;

	static rapidjson::Value GameObjectSerialize(const void *gameObjectPtr, rapidjson::Document::AllocatorType& allocator);
	static void GameObjectDeserializeAssign(void *gameObjectPtr, rapidjson::Value& jsonValue);

	META_REGISTER(GameObject)
	{
		META_DefineType(GameObject);
		META_DefineSerializeFunction(GameObject, GameObject::GameObjectSerialize);
	}
};

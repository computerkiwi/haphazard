/*
FILE: GameSpace.h
PRIMARY AUTHOR: Kieran

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

#include "GameObject.h"
#include "Component.h"

typedef void * ComponentType;

// Uses the address of the templated function as a unique ID.
template <typename T>
ComponentType GetComponentType()
{
	return reinterpret_cast<ComponentType>(GetComponentType<T>);
}

// Only exists so we can keep all of the component maps in one container.
class ComponentMapBase
{
public:
	~ComponentMapBase()
	{
	}

private:
};

template <typename T>
class ComponentMap : public ComponentMapBase
{
public:
	~ComponentMap()
	{
	}

	// Returns nullptr if it's not found.
	T *get(GameObject_ID id)
	{
		// Look for the object.
		auto iter = m_components.find(id);

		// If we couldn't find it, return nullptr. Else return pointer to it.
		if (iter == m_components.end())
		{
			return nullptr;
		}
		else
		{
			return &(*iter).second;
		}
	}

	template <typename... Args>
	void emplaceComponent(GameObject_ID id, Args&&... args)
	{
		m_components.emplace(id, T(std::forward<Args>(args)...));
	}
private:
	std::unordered_map<GameObject_ID, T> m_components;
};

class GameSpace
{
	template <typename T>
	friend class ComponentHandle;
	friend class GameObject;

public:
	template <typename T>
	void registerSystem()
	{
		m_componentMaps.emplace(GetComponentType<T>(), new ComponentMap<T>());
	}

	// Returns a component HANDLE.
	template <typename T>
	ComponentHandle<T> getComponent(GameObject_ID id)
	{
		return ComponentHandle<T>(id, this);
	}

	template <typename T>
	GameObject getGameObject(GameObject_ID id)
	{
		return GameObject(id, this);
	}

	GameObject NewGameObject()
	{
		return GameObject(GameObject::GenerateID(), this);
	}

private:
	template <typename T>
	T *getInternalComponent(GameObject_ID id)
	{
		// TODO[Kieran]: Cast individual components instead of the maps.

		ComponentMapBase *baseMap = m_componentMaps.at(GetComponentType<T>()).get();
		ComponentMap<T> *compMap = static_cast<ComponentMap<T> *>(baseMap);

		return compMap->get(id);
	}

	template <typename T, typename... Args>
	void emplaceComponent(GameObject_ID id, Args&&... args)
	{
		// TODO[Kieran]: Cast individual components instead of the maps.

		ComponentMapBase *baseMap = m_componentMaps.at(GetComponentType<T>()).get();
		ComponentMap<T> *compMap = static_cast<ComponentMap<T> *>(baseMap);

		compMap->emplaceComponent(id, std::forward<Args>(args)...);
	}

	std::unordered_map<ComponentType, std::unique_ptr<ComponentMapBase>> m_componentMaps;
};

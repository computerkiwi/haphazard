/*
FILE: GameSpace.h
PRIMARY AUTHOR: Kieran

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include <unordered_map>
#include <map>
#include <vector>
#include <memory>

#include "GameObject.h"
#include "Component.h"

// Forward declare.
class GameSpace;

// Required interface for systems.
class SystemBase
{
public:
	void RegisterGameSpace(GameSpace *space)
	{
		m_space = space;
	}

	SystemBase() : m_space(nullptr)
	{
	}

	// Called as the space initializes. Useful vs the constructor for timing purposes.
	virtual void Init() = 0;

	// Called each frame.
	virtual void Update(float dt) = 0;

	// Simply returns the default priority for this system.
	virtual size_t DefaultPriority() = 0;

protected:
	GameSpace *GetGameSpace()
	{
		return m_space;
	}
private:
	GameSpace *m_space;
};

// ID used as key to component containers.
typedef void * ComponentType;

// Uses the address of the templated function as a unique ID.
template <typename T>
struct GetComponentType
{
	constexpr static ComponentType func()
	{
		return static_cast<ComponentType>(func);
	}
};


// Only exists so we can keep all of the component maps in one container.
class ComponentMapBase
{
public:
	virtual ~ComponentMapBase()
	{
	}

private:
};

template <typename T>
class ComponentMap : public ComponentMapBase
{
public:
	virtual ~ComponentMap()
	{
	}

	ComponentMap(GameSpace *space) : m_space(space)
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
			return &iter->second;
		}
	}

	class iterator
	{
	public:
		iterator(typename std::unordered_map<GameObject_ID, T>::iterator iterator, GameSpace *space) : m_iterator(iterator), m_space(space)
		{
		}

		iterator& operator++()
		{
			++m_iterator;

			return *this;
		}

		iterator operator++(int)
		{
			iterator temp = *this;

			++m_iterator;

			return temp;
		}

		bool operator==(const iterator& other) const
		{
			return (m_iterator == other.m_iterator) && (m_space == other.m_space);
		}

		bool operator!=(const iterator& other) const
		{
			return !(*this == other);
		}

		ComponentHandle<T> operator*()
		{
			return ComponentHandle<T>(m_iterator->first, m_space);
		}

		ComponentHandle<T> operator->()
		{
			return ComponentHandle<T>(m_iterator->first, m_space);
		}

	private:
		typename std::unordered_map<GameObject_ID, T>::iterator m_iterator;
		GameSpace *m_space;
	};

	iterator begin()
	{
		return iterator(m_components.begin(), m_space);
	}

	iterator end()
	{
		return iterator(m_components.end(), m_space);
	}

	template <typename... Args>
	void emplaceComponent(GameObject_ID id, Args&&... args)
	{
		m_components.emplace(id, T(std::forward<Args>(args)...));
	}

private:
	std::unordered_map<GameObject_ID, T> m_components;
	GameSpace *m_space;
};

// Contains a container for each component type.
class GameSpace
{
	template <typename T>
	friend class ComponentHandle;
	friend class GameObject;

public:
	template <typename T>
	void registerComponentType()
	{
		m_componentMaps.emplace(GetComponentType<T>::func, new ComponentMap<T>(this));
	}

	void registerSystem(std::unique_ptr<SystemBase>&& newSystem, size_t priority)
	{
		newSystem->RegisterGameSpace(this);
		m_systems.insert(std::make_pair(priority, std::forward<std::unique_ptr<SystemBase>>(newSystem)));
	}
	void registerSystem(std::unique_ptr<SystemBase>&& newSystem)
	{
		registerSystem(std::move(newSystem), newSystem->DefaultPriority());
	}

	// Returns a component HANDLE.
	template <typename T>
	ComponentHandle<T> getComponent(GameObject_ID id)
	{
		return ComponentHandle<T>(id, this);
	}

	template <typename T>
	ComponentMap<T> *GetComponentMap()
	{
		ComponentMapBase *baseMap = m_componentMaps.at(GetComponentType<T>::func()).get();
		return static_cast<ComponentMap<T> *>(baseMap);
	}

	GameObject getGameObject(GameObject_ID id)
	{
		return GameObject(id, this);
	}

	GameObject NewGameObject()
	{
		return GameObject(GameObject::GenerateID(), this);
	}

	void Update(float dt)
	{
		for (auto iter = m_systems.begin(); iter != m_systems.end(); ++iter )
		{
			iter->second->Update(dt);
		}
	}

private:
	template <typename T>
	T *getInternalComponent(GameObject_ID id)
	{
		// TODO[Kieran]: Cast individual components instead of the maps.

		ComponentMapBase *baseMap = m_componentMaps.at(GetComponentType<T>::func()).get();
		ComponentMap<T> *compMap = static_cast<ComponentMap<T> *>(baseMap);

		return compMap->get(id);
	}

	template <typename T, typename... Args>
	void emplaceComponent(GameObject_ID id, Args&&... args)
	{
		// TODO[Kieran]: Cast individual components instead of the maps.

		ComponentMapBase *baseMap = m_componentMaps.at(GetComponentType<T>::func).get();
		ComponentMap<T> *compMap = static_cast<ComponentMap<T> *>(baseMap);

		compMap->emplaceComponent(id, std::forward<Args>(args)...);
	}

	std::unordered_map<ComponentType, std::unique_ptr<ComponentMapBase>> m_componentMaps;
	std::map<size_t, std::unique_ptr<SystemBase>> m_systems;
};

/*
FILE: GameSpace.h
PRIMARY AUTHOR: Kieran

Copyright � 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Universal.h"

#include <typeinfo>

#include <unordered_map>
#include <map>
#include <vector>
#include <memory>

#include "Component.h"
#include "TransformComponent.h"


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
	virtual std::size_t DefaultPriority() = 0;

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

	virtual void Duplicate(GameObject_ID originalObject, GameObject_ID newObject) = 0;

	virtual void Delete(GameObject_ID object) = 0;

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

	virtual void Duplicate(GameObject_ID originalObject, GameObject_ID newObject) override
	{
		if (m_components.find(originalObject) != m_components.end())
		{
			m_components.emplace(newObject, T(m_components.find(originalObject)->second));
		}
	}

	virtual void Delete(GameObject_ID object)
	{
		if (m_components.find(object) != m_components.end())
		{
			m_components.erase(object);
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
	void RegisterComponentType()
	{
		Logging::Log(Logging::CORE, Logging::MEDIUM_PRIORITY, "Gamespace ", this, " registering component type ", typeid(T).name());
		m_componentMaps.emplace(GetComponentType<T>::func, new ComponentMap<T>(this));
	}

	void RegisterSystem(std::unique_ptr<SystemBase>&& newSystem, std::size_t priority)
	{
		Logging::Log(Logging::CORE, Logging::MEDIUM_PRIORITY, "Gamespace ", this, " registering system");
		newSystem->RegisterGameSpace(this);
		m_systems.insert(std::make_pair(priority, std::move(newSystem)));
	}
	void RegisterSystem(std::unique_ptr<SystemBase>&& newSystem)
	{
		RegisterSystem(std::move(newSystem), newSystem->DefaultPriority());
	}

	void RegisterSystem(SystemBase *newSystem)
	{
		RegisterSystem(std::unique_ptr<SystemBase>(newSystem));
	}

	void RegisterSystem(SystemBase *newSystem, std::size_t priority)
	{
		RegisterSystem(std::unique_ptr<SystemBase>(newSystem), priority);
	}

	// Returns a component HANDLE.
	template <typename T>
	ComponentHandle<T> GetComponent(GameObject_ID id)
	{
		return ComponentHandle<T>(id, this);
	}

	template <typename T>
	ComponentMap<T> *GetComponentMap()
	{
		ComponentMapBase *baseMap = m_componentMaps.at(GetComponentType<T>::func()).get();
		return static_cast<ComponentMap<T> *>(baseMap);
	}

	GameObject GetGameObject(GameObject_ID id)
	{
		return GameObject(id, this);
	}

	GameObject NewGameObject()
	{
		return GameObject(GameObject::GenerateID(), this);
	}

	void Init()
	{
		for (auto iter = m_systems.begin(); iter != m_systems.end(); ++iter)
		{
			iter->second->Init();
		}
	}

	void Update(float dt)
	{
		for (auto iter = m_systems.begin(); iter != m_systems.end(); ++iter )
		{
			iter->second->Update(dt);
		}
	}

	GameObject Duplicate(GameObject_ID originalObject, GameObject_ID newObject)
	{
		for (auto& c_map : m_componentMaps)
		{
			c_map.second->Duplicate(originalObject, newObject);
		}

		return GameObject(newObject, this);
	}

	void Delete(GameObject_ID object)
	{
		for (auto& c_map : m_componentMaps)
		{
			c_map.second->Delete(object);
		}
	}

	std::vector<GameObject> CollectGameObjects()
	{
		std::vector<GameObject> objects;
		objects.reserve(30);
		auto *map = GetComponentMap<TransformComponent>();

		for (auto& transform : *map)
		{
			objects.emplace_back(transform.GetGameObject());
		}

		return std::move(objects);
	}

private:
	template <typename T>
	T *GetInternalComponent(GameObject_ID id)
	{
		// TODO[Kieran]: Cast individual components instead of the maps.

		ComponentMapBase *baseMap = m_componentMaps.at(GetComponentType<T>::func()).get();
		ComponentMap<T> *compMap = static_cast<ComponentMap<T> *>(baseMap);

		return compMap->get(id);
	}

	template <typename T, typename... Args>
	void EmplaceComponent(GameObject_ID id, Args&&... args)
	{
		// TODO[Kieran]: Cast individual components instead of the maps.

		ComponentMapBase *baseMap = m_componentMaps.at(GetComponentType<T>::func).get();
		ComponentMap<T> *compMap = static_cast<ComponentMap<T> *>(baseMap);

		compMap->emplaceComponent(id, std::forward<Args>(args)...);
	}

	std::unordered_map<ComponentType, std::unique_ptr<ComponentMapBase>> m_componentMaps;
	std::map<std::size_t, std::unique_ptr<SystemBase>> m_systems;
};


constexpr unsigned long hash(const char *str)
{
	unsigned long hash = 5381;
	int c = 0;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}


class GameSpaceManager
{
	std::unordered_map<std::size_t, GameSpace *> m_spaces;

	GameSpace *GetInteral(std::size_t key)
	{
		return m_spaces.at(key);
	}

public:
	void AddSpace(const char *name)
	{
		m_spaces.emplace(hash(name), new GameSpace());
	}

	inline GameSpace *Get(const char *name)
	{
		return GetInteral(hash(name));
	}

	inline GameSpace *operator[](const char *name)
	{
		return GetInteral(hash(name));
	}

	void Update(float dt)
	{

		for (auto sys : m_spaces)
		{
			sys.second->Update(dt);
		}
	}
};

#include "GameObject.h"

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
#include <iostream>

#include "Component.h"
#include "ObjectInfo.h"


// Forward declare.
class GameSpace;
class Engine;
extern Engine *engine;

// GameObject ID Gen
GameObject_ID GenerateID();
#define EXTRACTION_SHIFT (8 * 3)

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

	virtual ~SystemBase()
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
typedef std::size_t ComponentType;
typedef int GameSpaceIndex;

ComponentType GenerateComponentTypeID();

// Uses the address of the templated function as a unique ID.
template <typename T>
struct GetComponentType
{
	static ComponentType func()
	{
		static ComponentType cType = GenerateComponentTypeID();

		return cType;
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

	void DeleteComponent(GameObject_ID id)
	{
		m_components.erase(id);
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

	GameSpace() { RegisterComponentType<ObjectInfo>(); }
	explicit GameSpace(GameSpaceIndex index) : m_index(index) { RegisterComponentType<ObjectInfo>(); }

	template <typename T>
	void RegisterComponentType()
	{
		Logging::Log(Logging::CORE, Logging::MEDIUM_PRIORITY, "Gamespace ", this, " registering component type ", typeid(T).name());
		m_componentMaps.emplace(GetComponentType<T>::func(), new ComponentMap<T>(this));

		std::cout << "Registering component type" << GetComponentType<T>::func() << std::endl;
	}

	void RegisterSystem(SystemBase *newSystem, std::size_t priority)
	{
		Logging::Log(Logging::CORE, Logging::MEDIUM_PRIORITY, "Gamespace ", this, " registering system");
		newSystem->RegisterGameSpace(this);
		m_systems.insert(std::make_pair(priority, newSystem));
	}
	void RegisterSystem(SystemBase *newSystem)
	{
		RegisterSystem(newSystem, newSystem->DefaultPriority());
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
		ComponentMapBase *baseMap = m_componentMaps.at(GetComponentType<T>::func());
		return static_cast<ComponentMap<T> *>(baseMap);
	}

	template <typename T>
	void DeleteComponent(GameObject_ID id)
	{
		reinterpret_cast<ComponentMap<T> *>(m_componentMaps.at(GetComponentType<T>::func()))->DeleteComponent(id);
	}

	GameObject_ID GetGameObject() const
	{
		return GenerateID() | (m_index << EXTRACTION_SHIFT);
	}

	// GameObject_ID NewGameObject() const
	// {
	// 	return GenerateID() | (m_index << EXTRACTION_SHIFT);
	// }

	GameObject NewGameObject(const char *name) const
	{
		GameObject id = GenerateID() | (m_index << EXTRACTION_SHIFT);
		id.AddComponent<ObjectInfo>(id, name);
		return id;
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


	GameObject_ID Duplicate(GameObject_ID originalObject)
	{
		GameObject newObject = NewGameObject(GetComponent<ObjectInfo>(originalObject)->m_name.c_str());
		for (auto& c_map : m_componentMaps)
		{
			c_map.second->Duplicate(originalObject, newObject.Getid());
		}

		return newObject;
	}


	GameObject_ID Duplicate(GameObject_ID originalObject, GameObject_ID newObject)
	{
		for (auto& c_map : m_componentMaps)
		{
			c_map.second->Duplicate(originalObject, newObject);
		}

		return newObject;
	}

	void Delete(GameObject_ID object)
	{
		for (auto& c_map : m_componentMaps)
		{
			c_map.second->Delete(object);
		}
	}

	void CollectGameObjects(std::vector<GameObject_ID>& objects)
	{
		auto *map = GetComponentMap<ObjectInfo>();

		for (auto& transform : *map)
		{
			objects.emplace_back(transform.GetGameObject_ID());
		}
	}

private:
	template <typename T>
	T *GetInternalComponent(GameObject_ID id)
	{
		// TODO[Kieran]: Cast individual components instead of the maps.

		ComponentMapBase *baseMap = m_componentMaps.at(GetComponentType<T>::func());
		ComponentMap<T> *compMap = static_cast<ComponentMap<T> *>(baseMap);

		return compMap->get(id);
	}

	template <typename T, typename... Args>
	void EmplaceComponent(GameObject_ID id, Args&&... args)
	{
		// TODO[Kieran]: Cast individual components instead of the maps.

		ComponentMapBase *baseMap = m_componentMaps.at(GetComponentType<T>::func());
		ComponentMap<T> *compMap = static_cast<ComponentMap<T> *>(baseMap);

		compMap->emplaceComponent(id, std::forward<Args>(args)...);
	}
	GameSpaceIndex m_index = 0;
	std::unordered_map<ComponentType, ComponentMapBase *> m_componentMaps;
	std::map<std::size_t, SystemBase *> m_systems;
};


constexpr unsigned long hash(const char *str)
{
	unsigned long hash = 5381;
	int c = *str;

	while (c)
	{
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
		c = *str++;
	}

	return hash;
}


class GameSpaceManagerID
{
	std::vector<GameSpace> m_spaces;

public:
	void AddSpace()
	{
		if (m_spaces.size())
		{
			m_spaces.emplace_back(GameSpace(static_cast<int>(m_spaces.size())));
		}
		else
		{
			m_spaces.emplace_back(GameSpace(0));
		}
	}

	inline GameSpace *Get(std::size_t index)
	{
		return &m_spaces[index];
	}

	inline GameSpace *operator[](std::size_t index)
	{
		return &m_spaces[index];
	}

	void CollectAllObjects(std::vector<GameObject_ID>& objects)
	{
		objects.clear();
		for (int i = 0; i < m_spaces.size(); ++i)
		{
			m_spaces[i].CollectGameObjects(objects);
		}
	}

	void Update(float dt)
	{

		for (auto& sys : m_spaces)
		{
			sys.Update(dt);
		}
	}
};


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


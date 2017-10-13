/*
FILE: GameSpace.h
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Universal.h"

#include "meta/meta.h"

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
typedef std::size_t ComponentType;

ComponentType GenerateComponentTypeID();

#pragma optimize("", off)
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
#pragma optimize("", on)

// Only exists so we can keep all of the component maps in one container.
class ComponentMapBase
{
public:
	virtual ~ComponentMapBase()
	{
	}

	virtual void Duplicate(GameObject_ID originalObject, GameObject_ID newObject) = 0;

	virtual void Delete(GameObject_ID object) = 0;

	// This is a pointer, not a handle. It probably won't be valid very long.
	virtual meta::Any GetComponentPointerMeta(GameObject_ID object) = 0;

private:
};

// Standard ComponentMap template
// Any template specialization class must implement ALL of the functions in this class.
template <typename T>
class ComponentMap : public ComponentMapBase
{
public:
	virtual ~ComponentMap();

	ComponentMap(GameSpace *space);

	// Returns nullptr if it's not found.
	T *get(GameObject_ID id);

	virtual void Duplicate(GameObject_ID originalObject, GameObject_ID newObject) override;

	virtual void Delete(GameObject_ID object);

	virtual meta::Any GetComponentPointerMeta(GameObject_ID object);

	class iterator
	{
	public:
		iterator(typename std::unordered_map<GameObject_ID, T>::iterator iterator, GameSpace *space);

		iterator& operator++();

		iterator operator++(int);

		bool operator==(const iterator& other) const;

		bool operator!=(const iterator& other) const;

		ComponentHandle<T> operator*();

		ComponentHandle<T> operator->();

	private:
		typename std::unordered_map<GameObject_ID, T>::iterator m_iterator;
		GameSpace *m_space;
	};

	iterator begin();

	iterator end();

	template <typename... Args>
	void emplaceComponent(GameObject_ID id, Args&&... args);

	void DeleteComponent(GameObject_ID id);

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
	void RegisterComponentType();

	void RegisterSystem(std::unique_ptr<SystemBase>&& newSystem, std::size_t priority);
	void RegisterSystem(std::unique_ptr<SystemBase>&& newSystem);

	void RegisterSystem(SystemBase *newSystem);

	void RegisterSystem(SystemBase *newSystem, std::size_t priority);

	// Returns a component HANDLE.
	template <typename T>
	ComponentHandle<T> GetComponent(GameObject_ID id);

	template <typename T>
	ComponentMap<T> *GetComponentMap();

	template <typename T>
	void DeleteComponent(GameObject_ID id);

	GameObject GetGameObject(GameObject_ID id);

	GameObject NewGameObject();

	void Init();

	void Update(float dt);

	GameObject Duplicate(GameObject_ID originalObject, GameObject_ID newObject);

	void Delete(GameObject_ID object);

	std::vector<GameObject> CollectGameObjects();

	// These are pointers, not handles. They probably won't be valid very long.
	std::vector<meta::Any> GetObjectComponentPointersMeta(GameObject_ID id);

private:
	template <typename T>
	T *GetInternalComponent(GameObject_ID id);

	template <typename T, typename... Args>
	void EmplaceComponent(GameObject_ID id, Args&&... args);

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

	GameSpace *GetInteral(std::size_t key);

public:
	void AddSpace(const char *name);

	inline GameSpace *Get(const char *name)
	{
		return GetInteral(hash(name));
	}

	inline GameSpace *operator[](const char *name)
	{
		return GetInteral(hash(name));
	}

	void Update(float dt);
};

#include "GameSpace.inl"

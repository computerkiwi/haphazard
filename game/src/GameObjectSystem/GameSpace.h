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
#include <iostream>

#include "Component.h"
#include "ObjectInfo.h"
#include "TransformComponent.h"
#include "HierarchyComponent.h"

// Forward declare.
class GameSpace;
class Engine;
extern Engine *engine;

struct EditorAction;

// GameObject ID Gen
GameObject_ID GenerateID();
#define EXTRACTION_SHIFT (8 * 3)

// Required interface for systems.
class SystemBase
{
public:
	void RegisterGameSpace(GameSpaceIndex space)
	{
		m_space = space;
	}

	SystemBase() : m_space(0)
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

	virtual SystemBase *NewDuplicate() = 0;

protected:
	GameSpace *GetGameSpace() const;
private:
	GameSpaceIndex m_space;
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

	// This is a pointer, not a handle. It probably won't be valid very long.
	virtual meta::Any GetComponentPointerMeta(GameObject_ID object) = 0;

	virtual void UpdateSpaceIndex(GameSpaceIndex index) = 0;

	virtual ComponentMapBase *NewDuplicateMap() = 0;
};

// Forward declare
template <typename T>
class ComponentMap;

template <typename T>
void StaticUpdateSpaceIndex(ComponentMap<T>& map, GameSpaceIndex index);

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

	void Duplicate(GameObject_ID originalObject, GameObject_ID newObject) override;

	void Delete(GameObject_ID object) override;

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

	friend void StaticUpdateSpaceIndex<ObjectInfo>(ComponentMap<ObjectInfo>& map, GameSpaceIndex index);

	virtual void UpdateSpaceIndex(GameSpaceIndex index)
	{
		StaticUpdateSpaceIndex<T>(*this, index);
	}

private:
	std::unordered_map<GameObject_ID, T> m_components;
	GameSpace *m_space;

	virtual ComponentMapBase *NewDuplicateMap() override
	{
		return new ComponentMap<T>(*this);
	}
};

// UpdateSpaceIndex Functions
// TODO: Make this use SFINAE to check if the component has an UpdateSpaceIndex function.
template <typename T>
static void StaticUpdateSpaceIndex(ComponentMap<T>& map, GameSpaceIndex index)
{
	// Most don't need to do anything.
}

template <>
static void StaticUpdateSpaceIndex<ObjectInfo>(ComponentMap<ObjectInfo>& map, GameSpaceIndex index)
{
	for (auto& info : map)
	{
		info->m_id = GameObject::ConstructID(info->m_id, index);
	}
}

template <>
static void StaticUpdateSpaceIndex<TransformComponent>(ComponentMap<TransformComponent>& map, GameSpaceIndex index)
{
	for (auto& transform : map)
	{
		transform->SetParent(GameObject::ConstructID(transform->GetParent(), index));
	}
}

// Contains a container for each component type.
class GameSpace
{
	template <typename T>
	friend class ComponentHandle;
	friend class GameObject;

public:

	GameSpace() { RegisterInitial(); }
	explicit GameSpace(GameSpaceIndex index) : m_index(index) { RegisterInitial(); }
	
	GameSpace(const GameSpace& other);
	GameSpace(GameSpace&& other);
	GameSpace& operator=(const GameSpace& other);
	GameSpace& operator=(GameSpace&& other);

	~GameSpace();

	void SetIndex(GameSpaceIndex index);

	template <typename T>
	void RegisterComponentType();

	void RegisterSystem(SystemBase *newSystem);

	void RegisterSystem(SystemBase *newSystem, std::size_t priority);

	// Returns a component HANDLE.
	template <typename T>
	ComponentHandle<T> GetComponent(GameObject_ID id);

	template <typename T>
	ComponentMap<T> *GetComponentMap();

	template <typename T>
	void DeleteComponent(GameObject_ID id);

	GameObject GetGameObject(GameObject_ID id) const;

	GameObject NewGameObject(const char *name) const;

	void Init();

	void Update(float dt);

	GameObject Duplicate(GameObject_ID originalObject, GameObject_ID newObject);

	void Delete(GameObject_ID object);
	
	// Delimits between Spaces by adding an invalid ID
	void CollectGameObjectsDelimited(std::vector<GameObject_ID>& objects);

	std::vector<GameObject> CollectGameObjects();
	void CollectGameObjects(std::vector<GameObject_ID>& objects);

	// On the default engine.
	static GameSpace *GetByIndex(GameSpaceIndex index);

	// These are pointers, not handles. They probably won't be valid very long.
	std::vector<meta::Any> GetObjectComponentPointersMeta(GameObject_ID id);

private:
	void RegisterInitial();

	template <typename T>
	T *GetInternalComponent(GameObject_ID id);

	GameSpaceIndex  m_index;

	template <typename T, typename... Args>
	void EmplaceComponent(GameObject_ID id, Args&&... args);

	void AddComponentMeta(GameObject_ID id, meta::Any& component);

	std::unordered_map<ComponentType, ComponentMapBase *> m_componentMaps;
	std::map<std::size_t, SystemBase *> m_systems;

	static rapidjson::Value GameSpaceSerialize(const void *gameSpacePtr, rapidjson::Document::AllocatorType& allocator)
	{
		// Const cast away is fine because we're not really changing anything.
		GameSpace& gameSpace = *reinterpret_cast<GameSpace *>(const_cast<void *>(gameSpacePtr));

		// Setup the array to put the objects into.
		rapidjson::Value gameObjectArray;
		gameObjectArray.SetArray();

		// Get all the GameObjects.
		std::vector<GameObject> objects = gameSpace.CollectGameObjects();
		// Put the objects in the json array.
		for (const auto& gameObject : objects)
		{
			gameObjectArray.PushBack(gameObject.SerializeObject(allocator), allocator);
		}

		return gameObjectArray;
	}

	static void GameSpaceDeserializeAssign(void *spacePtr, rapidjson::Value& jsonSpace)
	{
		// Get the space.
		GameSpace& gameSpace = *reinterpret_cast<GameSpace *>(spacePtr);

		// The json space should be an array of GameObjects
		assert(jsonSpace.IsArray());

		// Deserialize each GameObject.
		for (auto& jsonGameObject : jsonSpace.GetArray())
		{
			// Setup the gamespace to use and deserialize the object.
			GameObject::SetDeserializeSpace(gameSpace.m_index);
			GameObject obj = gameSpace.NewGameObject("");
			obj.DeserializeObject(jsonGameObject);
		}
	}

	META_REGISTER(GameSpace)
	{
		META_DefineSerializeFunction(GameSpace, GameSpaceSerialize);
		META_DefineDeserializeAssignFunction(GameSpace, GameSpaceDeserializeAssign);
	}
};


class GameSpaceManagerID
{
	std::vector<GameSpace> m_spaces;

public:
	size_t GetSize() const
	{
		return m_spaces.size();
	}

	inline std::vector<GameSpace>& GetSpaces()
	{
		return m_spaces;
	}

	// Returns index of the new space.
	GameSpaceIndex AddSpace()
	{
		int index;

		if (m_spaces.size())
		{
			index = static_cast<int>(m_spaces.size());
		}
		else
		{
			index = 0;
		}

		m_spaces.emplace_back(GameSpace(index));
		return index;
	}

	void AddSpace(GameSpace& gameSpace)
	{
		if (m_spaces.size())
		{
			gameSpace.SetIndex(static_cast<GameSpaceIndex>(m_spaces.size()));
		}
		else
		{
			gameSpace.SetIndex(0);
		}

		m_spaces.push_back(gameSpace);
	}

	inline GameSpace &Get(std::size_t index)
	{
		return m_spaces[index];
	}

	inline GameSpace *operator[](std::size_t index)
	{
		return &m_spaces[index];
	}

	void ClearSpaces()
	{
		m_spaces.clear();
	}

	void CollectAllObjects(std::vector<GameObject_ID>& objects)
	{
		objects.clear();
		for (int i = 0; i < m_spaces.size(); ++i)
		{
			m_spaces[i].CollectGameObjects(objects);
		}
	}

	void CollectAllObjectsDelimited(std::vector<GameObject_ID>& objects)
	{
		objects.clear();
		for (int i = 0; i < m_spaces.size(); ++i)
		{
			m_spaces[i].CollectGameObjectsDelimited(objects);
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

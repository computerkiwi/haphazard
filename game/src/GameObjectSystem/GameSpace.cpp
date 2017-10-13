/*
FILE: GameSpace.h
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/

#include "GameSpace.h"

static ComponentType lastID = 0;

ComponentType GenerateComponentTypeID()
{
	return lastID++;
}

//-----------
// GameSpace
//-----------

void GameSpace::RegisterSystem(std::unique_ptr<SystemBase>&& newSystem, std::size_t priority)
{
	Logging::Log(Logging::CORE, Logging::MEDIUM_PRIORITY, "Gamespace ", this, " registering system");
	newSystem->RegisterGameSpace(this);
	m_systems.insert(std::make_pair(priority, std::move(newSystem)));
}

void GameSpace::RegisterSystem(std::unique_ptr<SystemBase>&& newSystem)
{
	RegisterSystem(std::move(newSystem), newSystem->DefaultPriority());
}

void GameSpace::RegisterSystem(SystemBase *newSystem)
{
	RegisterSystem(std::unique_ptr<SystemBase>(newSystem));
}

void GameSpace::RegisterSystem(SystemBase *newSystem, std::size_t priority)
{
	RegisterSystem(std::unique_ptr<SystemBase>(newSystem), priority);
}

GameObject GameSpace::GetGameObject(GameObject_ID id)
{
	return GameObject(id, this);
}

GameObject GameSpace::NewGameObject()
{
	return GameObject(GameObject::GenerateID(), this);
}

void GameSpace::Init()
{
	for (auto iter = m_systems.begin(); iter != m_systems.end(); ++iter)
	{
		iter->second->Init();
	}
}

void GameSpace::Update(float dt)
{
	for (auto iter = m_systems.begin(); iter != m_systems.end(); ++iter)
	{
		iter->second->Update(dt);
	}
}

GameObject GameSpace::Duplicate(GameObject_ID originalObject, GameObject_ID newObject)
{
	for (auto& c_map : m_componentMaps)
	{
		c_map.second->Duplicate(originalObject, newObject);
	}

	return GameObject(newObject, this);
}

void GameSpace::Delete(GameObject_ID object)
{
	for (auto& c_map : m_componentMaps)
	{
		c_map.second->Delete(object);
	}
}

std::vector<GameObject> GameSpace::CollectGameObjects()
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

// These are pointers, not handles. They probably won't be valid very long.
std::vector<meta::Any> GameSpace::GetObjectComponentPointersMeta(GameObject_ID id)
{
	std::vector<meta::Any> components;

	// Get the component from each map.
	for (auto& mapPair : m_componentMaps)
	{
		meta::Any componentPtr = mapPair.second->GetComponentPointerMeta(id);
		if (!componentPtr.IsNullPtr())
		{
			components.push_back(componentPtr);
		}
	}

	return components;
}

//------------------
// GameSpaceManager
//------------------

GameSpace *GameSpaceManager::GetInteral(std::size_t key)
{
	return m_spaces.at(key);
}

void GameSpaceManager::AddSpace(const char *name)
{
	m_spaces.emplace(hash(name), new GameSpace());
}

void GameSpaceManager::Update(float dt)
{

	for (auto sys : m_spaces)
	{
		sys.second->Update(dt);
	}
}

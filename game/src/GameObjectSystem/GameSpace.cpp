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


GameObject_ID GenerateID()
{
	static GameObject_ID lastGeneratedID = 1;

	return lastGeneratedID++;
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

GameObject GameSpace::GetGameObject(GameObject_ID id) const
{
	return GameObject(id, m_index);
}

GameObject GameSpace::NewGameObject(const char *name) const
{
	GameObject object(GenerateID() | (m_index << EXTRACTION_SHIFT));
	object.AddComponent<ObjectInfo>(name);
	return object;
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

	return GameObject(newObject, m_index);
}

void GameSpace::Delete(GameObject_ID object)
{
	for (auto& c_map : m_componentMaps)
	{
		c_map.second->Delete(object);
	}
}

void GameSpace::CollectGameObjects(std::vector<GameObject_ID>& objects)
{
	auto *map = GetComponentMap<ObjectInfo>();

	for (auto& transform : *map)
	{
		objects.emplace_back(transform.GetGameObject());
	}
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

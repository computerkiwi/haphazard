/*
FILE: GameSpace.h
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/

#include "GameSpace.h"
#include "Engine\Engine.h"

// Component types to register.
#include "GameObjectSystem/TransformComponent.h"
#include "graphics\SpriteComponent.h"
#include "graphics\Texture.h"
#include "Engine\Physics\RigidBody.h"
#include "Engine\Physics\Collider2D.h"
#include "Scripting\ScriptComponent.h"
#include "graphics\Camera.h"
#include "graphics\Particles.h"
#include "graphics\Text.h"
#include "graphics\Background.h"

// Systems to register.
#include "graphics\RenderSystem.h"
#include "Engine\Physics\PhysicsSystem.h"
#include "Scripting\ScriptSystem.h"

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


template <>
void ComponentMap<HierarchyComponent>::Delete(GameObject_ID object)
{
	if (m_components.find(object) != m_components.end())
	{
		std::vector<GameObject>& children = m_components.find(object)->second.GetList();
		for (auto child : children)
		{
			child.GetComponent<TransformComponent>()->SetParent(0);
		}

		m_components.erase(object);
	}
}


void GameSpace::RegisterSystem(SystemBase *newSystem, std::size_t priority)
{
	Logging::Log(Logging::CORE, Logging::MEDIUM_PRIORITY, "Gamespace ", this, " registering system");
	newSystem->RegisterGameSpace(m_index);
	//m_systems.insert(std::make_pair(priority, std::move(*newSystem)));
	m_systems.emplace(priority, newSystem);
}

void GameSpace::SetIndex(GameSpaceIndex index)
{
	m_index = index;

	for (auto& mapPair : m_componentMaps)
	{
		mapPair.second->UpdateSpaceIndex(index);
	}
}

void GameSpace::RegisterSystem(SystemBase *newSystem)
{
	RegisterSystem(newSystem, newSystem->DefaultPriority());
}

GameObject GameSpace::GetGameObject(GameObject_ID id) const
{
	return GameObject(id, m_index);
}

GameObject GameSpace::NewGameObject(const char *name) const
{
	GameObject object(GenerateID() | (m_index << EXTRACTION_SHIFT));
	object.AddComponent<ObjectInfo>(object.Getid(), name);
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

std::vector<GameObject> GameSpace::CollectGameObjects()
{
	// Get the object ids.
	std::vector<GameObject_ID> ids;
	CollectGameObjects(ids);

	// Make them into GameObjects.
	std::vector<GameObject> objects;
	for (GameObject_ID id : ids)
	{
		objects.emplace_back(id);
	}

	return objects;
}


// Delimits between Spaces by adding an invalid ID
void GameSpace::CollectGameObjectsDelimited(std::vector<GameObject_ID>& objects)
{
	auto *map = GetComponentMap<ObjectInfo>();

	for (auto& info : *map)
	{
		objects.emplace_back(info.GetGameObject().Getid());
	}

	// Delimits between Spaces
	objects.emplace_back(INVALID_GAMEOBJECT_ID);
}


void GameSpace::CollectGameObjects(std::vector<GameObject_ID>& objects)
{
	auto *map = GetComponentMap<ObjectInfo>();

	for (auto& info : *map)
	{
		objects.emplace_back(info.GetGameObject().Getid());
	}
}


GameSpace *GameSpace::GetByIndex(GameSpaceIndex index)
{
	return engine->GetSpace(index);
}

GameSpace::GameSpace(const GameSpace& other) : m_index(other.m_index)
{
	for (auto& sysPair : other.m_systems)
	{
		m_systems.insert(std::make_pair(sysPair.first, sysPair.second->NewDuplicate()));
	}

	for (auto& componentMapPair : other.m_componentMaps)
	{
		m_componentMaps.insert(std::make_pair(componentMapPair.first, componentMapPair.second->NewDuplicateMap()));
	}
}

GameSpace::GameSpace(GameSpace&& other) : m_index(other.m_index)
{
	std::swap(m_componentMaps, other.m_componentMaps);
	std::swap(m_systems, other.m_systems);
}

GameSpace& GameSpace::operator=(const GameSpace& other)
{
	m_index = other.m_index;

	// Clear out the current maps.
	for (auto& sys : m_systems)
	{
		delete sys.second;
	}
	for (auto& componentMap : m_componentMaps)
	{
		delete componentMap.second;
	}
	// Copy values in.
	for (auto& sysPair : other.m_systems)
	{
		m_systems.insert(std::make_pair(sysPair.first, sysPair.second->NewDuplicate()));
	}
	for (auto& componentMapPair : other.m_componentMaps)
	{
		m_componentMaps.insert(std::make_pair(componentMapPair.first, componentMapPair.second->NewDuplicateMap()));
	}

	return *this;
}

GameSpace& GameSpace::operator=(GameSpace && other)
{
	m_index = other.m_index;
	std::swap(m_componentMaps, other.m_componentMaps);
	std::swap(m_systems, other.m_systems);

	return *this;
}

GameSpace::~GameSpace()
{
	for (auto& sys : m_systems)
	{
		delete sys.second;
	}

	for (auto& componentMap : m_componentMaps)
	{
		delete componentMap.second;
	}
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

void GameSpace::RegisterInitial()
{
	RegisterComponentType<ObjectInfo>();
	RegisterComponentType<TransformComponent>();
	RegisterComponentType<RigidBodyComponent>();
	RegisterComponentType<StaticCollider2DComponent>();
	RegisterComponentType<DynamicCollider2DComponent>();
	RegisterComponentType<SpriteComponent>();
	RegisterComponentType<ScriptComponent>();
	RegisterComponentType<ParticleSystem>();
	RegisterComponentType<Camera>();
	RegisterComponentType<TextComponent>();
	RegisterComponentType<HierarchyComponent>();
	RegisterComponentType<BackgroundComponent>();

	RegisterSystem(new PhysicsSystem());
	RegisterSystem(new RenderSystem());
	RegisterSystem(new ScriptSystem());
}

// Helper macro for GameSpace::AddComponentMeta
#define IfAddComponent(TYPE)\
if (type == meta::GetTypePointer<TYPE>())\
{\
	EmplaceComponent<TYPE>(id, component.GetData<TYPE>());\
}\

void GameSpace::AddComponentMeta(GameObject_ID id, meta::Any& component)
{
	meta::Type *type = component.GetType();

	IfAddComponent(TransformComponent) else
	IfAddComponent(SpriteComponent) else
	IfAddComponent(RigidBodyComponent) else
	IfAddComponent(StaticCollider2DComponent) else
	IfAddComponent(DynamicCollider2DComponent) else
	IfAddComponent(ObjectInfo) else
	IfAddComponent(ScriptComponent) else
	IfAddComponent(Camera) else
	IfAddComponent(TextComponent) else
	IfAddComponent(ParticleSystem)
	else
	{
		Logging::Log(Logging::Channel::META, Logging::CRITICAL_PRIORITY, "Found unkown component type \"", type->GetName(), "\" when adding an Any component to a space.");
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

GameSpace *SystemBase::GetGameSpace() const
{
	return GameSpace::GetByIndex(m_space);
}


// Template Specialization
template <>
void ComponentMap<ObjectInfo>::Duplicate(GameObject_ID originalObject, GameObject_ID newObject)
{
	if (m_components.find(originalObject) != m_components.end())
	{
		m_components.emplace(newObject, ObjectInfo(newObject, m_components.find(originalObject)->second));
	}
}


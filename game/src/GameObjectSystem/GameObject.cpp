/*
FILE: GameObject.cpp
PRIMARY AUTHOR: Kieran

Copyright � 2017 DigiPen (USA) Corporation.
*/

#include "Engine\Engine.h"
#include "GameObject.h"

extern Engine *engine;

GameObject::GameObject() : m_objID(INVALID_GAMEOBJECT_ID)
{
}

GameObject::GameObject(int id, GameSpaceIndex gameSpace) : m_id(id), m_space(gameSpace)
{
}


implicit GameObject::GameObject(GameObject_ID id) : m_objID(id)
{
}

// Tries to add a component by type.
void GameObject::AddComponent(meta::Any& component)
{
	GetSpace()->AddComponentMeta(m_objID, component);
}

GameObject_ID GameObject::GetObject_id() const
{
	return m_id;
}


GameObject_ID GameObject::Getid() const
{
	return m_objID;
}


GameSpace *GameObject::GetSpace() const
{
	assert(IsValid());
	// (0xFF00000000000000 & m_objID) >> EXTRACTION_SHIFT
	return engine->GetSpace(m_space);
}


GameSpaceIndex GameObject::GetIndex() const
{
	assert(IsValid());
	return m_space; // m_objID & 0xFF00000000000000;
}


void GameObject::SetSpace(GameSpaceIndex index)
{
	m_space = index;
}


GameObject_ID GameObject::Duplicate() const
{
	return GetSpace()->Duplicate(m_objID, GenerateID());
}


void GameObject::Delete()
{
	GetSpace()->Delete(m_objID);
	m_objID = 0;
}

rapidjson::Value GameObject::GameObjectSerialize(const void *gameObjectPtr, rapidjson::Document::AllocatorType& allocator)
{
	const GameObject& gameObject = *reinterpret_cast<const GameObject *>(gameObjectPtr);
	assert(gameObject.IsValid());

	// Make the json object.
	rapidjson::Value jsonObject;
	jsonObject.SetObject();

	// Add the object id.
	rapidjson::Value jsonObjID;
	jsonObjID.SetInt64(gameObject.m_objID);
	jsonObject.AddMember("objID", jsonObjID, allocator);

	// Construct and add the array of components.
	rapidjson::Value componentArray;
	componentArray.SetArray();
	std::vector<meta::Any> components = gameObject.GetSpace()->GetObjectComponentPointersMeta(gameObject.m_objID);
	for (meta::Any& component : components)
	{
		componentArray.PushBack(component.Serialize(allocator), allocator);
	}
	jsonObject.AddMember("componentArray", componentArray, allocator);

	return jsonObject;
}

// This is very not threadsafe. Should probably be called before each deserialize.
static GameSpaceIndex deserializeGameSpace = 0;
void GameObject::SetDeserializeSpace(GameSpaceIndex index)
{
	deserializeGameSpace = 0;
}

rapidjson::Value GameObject::SerializeObject(rapidjson::Document::AllocatorType& allocator) const
{
	return GameObjectSerialize(this, allocator);
}

void GameObject::DeserializeObject(rapidjson::Value& jsonValue)
{
	GameObjectDeserializeAssign(this, jsonValue);
}

void GameObject::GameObjectDeserializeAssign(void *gameObjectPtr, rapidjson::Value& jsonValue)
{
	GameObject& gameObject = *reinterpret_cast<GameObject *>(gameObjectPtr);

	// We should be assigning to an invalid GameObject.
	assert(!gameObject.IsValid());

	// Assertions saying we have the right type of value.
	assert(jsonValue.IsObject());
	assert(jsonValue.HasMember("objID"));
	assert(jsonValue.HasMember("componentArray"));

	// Get the id from the json.
	rapidjson::Value jsonId;
	jsonId = jsonValue["objID"];
	assert(jsonId.IsInt64());
	GameObject_ID id = jsonId.GetInt64();

	// Setup the GameObject's id.
	gameObject.m_objID = id;
	gameObject.m_space = deserializeGameSpace; // From static function. Must be called before this.

	// Get the array of components.
	rapidjson::Value componentArray;
	componentArray = jsonValue["componentArray"];
	assert(componentArray.IsArray());

	// Load each component.
	for (auto& jsonComponent : componentArray.GetArray())
	{
		// Pull out the component and its type.
		meta::Any anyComponent(jsonComponent);

		// Add it to the space.
		gameObject.AddComponent(anyComponent);
	}
}

std::vector<meta::Any> GameObject::GetComponentPointersMeta()
{
	return GetSpace()->GetObjectComponentPointersMeta(m_objID);
}

bool GameObject::IsValid() const
{
	return (m_objID != INVALID_GAMEOBJECT_ID);
}

GameObject_ID GameObject::ConstructID(int id, GameSpaceIndex spaceIndex)
{
	// Set up the bitfields to make it easy.
	union
	{
		struct
		{
			int id : 24;
			int space : 8;
		};
		GameObject_ID objID;
	} helper;

	helper.id = id;
	helper.space = spaceIndex;
	return helper.objID;
}



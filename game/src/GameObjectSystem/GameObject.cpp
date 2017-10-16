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

GameObject::GameObject(GameObject_ID id, GameSpaceIndex gameSpace) : m_objID(id & (gameSpace << EXTRACTION_SHIFT))
{
}


implicit GameObject::GameObject(GameObject_ID id) : m_objID(id)
{
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

void GameObject::GameObjectDeserializeAssign(void *gameObjectPtr, rapidjson::Value& jsonValue)
{
	const GameObject& gameObject = *reinterpret_cast<const GameObject *>(gameObjectPtr);

	// Assertions saying we have the right type of value.
	assert(jsonValue.IsObject());
	assert(jsonValue.HasMember("objID"));
	assert(jsonValue.HasMember("componentArray"));
}

std::vector<meta::Any> GameObject::GetComponentPointersMeta()
{
	return GetSpace()->GetObjectComponentPointersMeta(m_objID);
}

bool GameObject::IsValid() const
{
	return (m_objID != INVALID_GAMEOBJECT_ID);
}



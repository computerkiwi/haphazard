/*
FILE: GameObject.h
PRIMARY AUTHOR: Kieran

Copyright © 2017 DigiPen (USA) Corporation.
*/

#include "GameObject.h"
#include "GameSpace.h"

GameObject_ID GameObject::lastGeneratedID = 0;

GameObject_ID GameObject::GenerateID()
{
	return lastGeneratedID++;
}

void GameObject::SetHighestID(GameObject_ID highestID)
{
	if (highestID > lastGeneratedID)
	{
		highestID = lastGeneratedID;
	}
}

rapidjson::Value GameObject::GameObjectSerialize(const void *gameObjectPtr, rapidjson::Document::AllocatorType& allocator)
{
	const GameObject& gameObject = *reinterpret_cast<const GameObject *>(gameObjectPtr);

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
	std::vector<meta::Any> components = gameObject.m_gameSpace->GetObjectComponentPointersMeta(gameObject.m_objID);
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
	return m_gameSpace->GetObjectComponentPointersMeta(m_objID);
}

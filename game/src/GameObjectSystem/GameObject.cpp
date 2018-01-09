/*
FILE: GameObject.cpp
PRIMARY AUTHOR: Kieran

Copyright � 2017 DigiPen (USA) Corporation.
*/

#include "Engine\Engine.h"
#include "GameObject.h"

#include "Scripting/ScriptComponent.h"

#include "Util/Serialization.h"

#include "Engine/Physics/Collider2D.h"

extern Engine *engine;

GameObject::GameObject() : m_id(0), m_space(0)
{
}

GameObject::GameObject(GameObject_ID id, GameSpaceIndex gameSpace) : m_id(id), m_space(gameSpace)
{
}


implicit GameObject::GameObject(GameObject_ID id) : m_objID(id)
{
}


// Tries to add a component by type.
void GameObject::AddComponent(meta::Any& component) const
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

std::string GameObject::GetName() const
{
	return GetComponent<ObjectInfo>()->m_name;
}

void GameObject::SetName(const std::string & name) const
{
	GetComponent<ObjectInfo>()->m_name = name;
}


GameObject_ID GameObject::Duplicate() const
{
	return GetSpace()->Duplicate(m_objID, GenerateID());
}


void GameObject::DeleteInternal()
{
	GetSpace()->Delete(m_objID);
}


void GameObject::Delete()
{
	Destroy();
}


bool GameObject::IsValid() const
{
	if (m_objID == INVALID_GAMEOBJECT_ID)
	{
		return false;
	}

	// GetComponent checks IsValid
	return engine->GetSpace(m_space)->GetInternalComponent<ObjectInfo>(m_objID) != nullptr;
}


bool GameObject::Usable() const
{
	return IsValid() && IsAlive();
}


bool GameObject::IsActive() const
{
	return GetComponent<ObjectInfo>()->m_active;
}


void GameObject::Activate() const
{
	GetComponent<ObjectInfo>()->m_active = true;
}


void GameObject::Deactivate() const
{
	GetComponent<ObjectInfo>()->m_active = false;
}


void GameObject::On() const
{
	Activate();
}


void GameObject::Off() const
{
	Deactivate();
}


void GameObject::SetActive(bool state) const
{
	GetComponent<ObjectInfo>()->m_active = state;
}


bool GameObject::IsDestroyed() const
{
	return GetComponent<ObjectInfo>()->m_destroyed;
}


bool GameObject::IsAlive() const
{
	return !(GetComponent<ObjectInfo>()->m_destroyed);
}



void GameObject::Destroy() const
{
	GetComponent<ObjectInfo>()->m_destroyed = true;
}


void GameObject::Restore() const
{
	GetComponent<ObjectInfo>()->m_destroyed = false;
}


void GameObject::SetDestroy(bool state) const
{
	GetComponent<ObjectInfo>()->m_destroyed = state;
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


GameObject GameObject::FindByName(const char * name)
{
	// TODO: Make this deal with multiple spaces.
	for (auto info : *engine->GetSpace(0)->GetComponentMap<ObjectInfo>())
	{
		if (info->m_name == name)
		{
			return GameObject(info->m_id);
		}
	}
	return GameObject();
}


GameObject GameObject::FindByTag(const char * tagStr)
{
	size_t object_tag = hash(tagStr);
	// TODO: Make this deal with multiple spaces.
	for (auto& info : *engine->GetSpace(0)->GetComponentMap<ObjectInfo>())
	{
		for (auto& tag : info->m_tags)
		{
			if (tag.first == object_tag)
			{
				return GameObject(info->m_id);
			}
		}
	}
	return GameObject();
}


void GameObject::Create(const char *name)
{
	GameObject object = engine->GetSpace(0)->NewGameObject(name);
	object.AddComponent<TransformComponent>();
}


void GameObject::CreateToSpace(const char *name, GameSpaceIndex index)
{
	GameObject object = engine->GetSpace(index)->NewGameObject(name);
	object.AddComponent<TransformComponent>();
}


std::vector<GameObject> GameObject::FindAllByTag(const char *tagStr)
{
	std::vector<GameObject> objects;
	
	size_t object_tag = hash(tagStr);

	for (auto& info : *engine->GetSpace(0)->GetComponentMap<ObjectInfo>())
	{
		for (auto& tag : info->m_tags)
		{
			if (tag.first == object_tag)
			{
				objects.emplace_back(info->m_id);
			}
		}
	}

	return objects;
}


void GameObject::SaveToFile(const char * fileName)
{
	// Make a document for the allocator.
	// TODO: Figure out how to get an allocator without bothering with a whole document.
	rapidjson::Document doc;

	JsonToFile(this->SerializeObject(doc.GetAllocator()), fileName);
}

GameObject GameObject::LoadPrefab(const char * fileName)
{
	ClearSerializedIdRelationships();
	auto jsonObj = LoadJsonFile(fileName);

	GameObject obj = engine->GetSpace(0)->NewGameObject("");
	obj.DeserializeObject(jsonObj);
	ApplySerializedIdUpdates();

	return obj;
}

bool GameObject::HasTag(const char * tagName)
{
	return GetComponent<ObjectInfo>()->HasTag(tagName);
}


void GameObject::GameObjectDeserializeAssign(void *gameObjectPtr, rapidjson::Value& jsonValue)
{
	GameObject& gameObject = *reinterpret_cast<GameObject *>(gameObjectPtr);

	// We should be assigning to a valid GameObject.
	assert(gameObject.IsValid());

	// Assertions saying we have the right type of value.
	assert(jsonValue.IsObject());
	assert(jsonValue.HasMember("objID"));
	assert(jsonValue.HasMember("componentArray"));

	// Get the id from the json.
	rapidjson::Value jsonId;
	jsonId = jsonValue["objID"];
	assert(jsonId.IsInt64());
	GameObject_ID oldId = static_cast<GameObject_ID>(jsonId.GetInt64());

	// Keep track of which old id corresponds to which new id.
	RegisterSerializedIdRelationship(oldId, gameObject.m_id);

	// Get the array of components.
	rapidjson::Value componentArray;
	componentArray = jsonValue["componentArray"];
	assert(componentArray.IsArray());

	// Load each component.
	for (auto& jsonComponent : componentArray.GetArray())
	{
		// Pull out the component and its type.
		meta::Any anyComponent(jsonComponent);

		anyComponent.SetGameObjectID(gameObject.Getid());

		// Add it to the space.
		gameObject.AddComponent(anyComponent);
	}

	// Setup a functor to set transform parents later.
	if (gameObject.GetComponent<TransformComponent>()->GetParent().Getid() != INVALID_GAMEOBJECT_ID)
	{
		// Lambda that looks for an old parent id to correspond to a new one.
		RegisterSerializedIdUpdate([gameObject](std::map<GameObject_ID, GameObject_ID> idMap)
		{
			for (const auto& idPair : idMap)
			{
				ComponentHandle<TransformComponent> transform = gameObject.GetComponent<TransformComponent>();
				if (transform->GetParent() == idPair.first)
				{
					// The "Lua" functions adds a needed hierarchy component to the parent if it doesn't exist.
					transform->SetParentLua(idPair.second);
					return;
				}
			}
		});
	}

}

Collider2D *GameObject::GetCollider()
{
	ComponentHandle<DynamicCollider2DComponent> component = GetComponent<DynamicCollider2DComponent>();
	if (component.IsValid())
	{
		return &component->ColliderData();
	}
	else
	{
		ComponentHandle<StaticCollider2DComponent> staticComponent = GetComponent<StaticCollider2DComponent>();
		if (staticComponent.IsValid())
		{
			return &staticComponent->ColliderData();
		}
		else
		{
			return nullptr;
		}
	}
}

// Takes 1 parameter: string fileName
int GameObject::GetScript(lua_State * L)
{
	// Expecting two parameters. This gameobject and filename string.
	int temp = lua_gettop(L);
	assert(temp == 2);

	// Pull out the filename string.
	const char *fileName = luaL_checkstring(L, 2);

	// Get the scriptcomponent and check for valididity.
	ComponentHandle<ScriptComponent> handle = GetComponent<ScriptComponent>();
	if (!handle.IsValid())
	{
		lua_pushnil(L);
		return 1;
	}

	// Get the script and check for valididity.
	LuaScript *script = handle->GetScriptByFilename(fileName);
	if (script == nullptr)
	{
		lua_pushnil(L);
		return 1;
	}

	// Everything's valid, so let the script give us its environment.
	return script->GetScriptEnvironmentLua(L);
}

std::vector<meta::Any> GameObject::GetComponentPointersMeta()
{
	return GetSpace()->GetObjectComponentPointersMeta(m_objID);
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



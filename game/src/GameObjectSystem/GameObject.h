/*
FILE: GameObject.h
PRIMARY AUTHOR: Kieran

Copyright � 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "meta/meta.h"

class GameSpace;

extern GameSpace *defaultGameSpacePtr;

#define implicit

template <typename T>
class ComponentHandle;


typedef int GameObject_ID;
typedef int GameSpaceIndex;
typedef int    dummy;

const GameObject_ID INVALID_GAMEOBJECT_ID = 0;

class GameObject
{
public:
	GameObject();

	GameObject(GameObject_ID id, GameSpaceIndex gameSpace);

	implicit GameObject(GameObject_ID id);

	template <typename T, typename... Args>
	void AddComponent(Args&&... args)
	{
		GetSpace()->EmplaceComponent<T>(m_objID, std::forward<Args>(args)...);
	}

	void AddComponent(meta::Any& component);

	template <typename T>
	ComponentHandle<T> GetComponent() const
	{
		assert(IsValid());

		// Make sure the component exists before we hand it off.
		if (GetSpace()->GetInternalComponent<T>(m_objID) != nullptr)
		{
			// Why the hell does constructing a ComponentHandle work?
			// We never forward declare the constructor and we shouldn't know what sizeof(ComponentHandle<T>) is, right? -Kieran

			// TEMPLATES -Sweet
			return ComponentHandle<T>(m_objID);
		}
		else
		{
			return ComponentHandle<T>(0, false);
		}
	}

	GameObject_ID GetObject_id() const;

	GameObject_ID Getid() const;

	GameObject_ID Duplicate() const;

	void Delete();

	GameSpace *GetSpace() const;

	GameSpaceIndex GetIndex() const;

	void SetSpace(GameSpaceIndex index);

	std::string GetName();
	void SetName(const std::string& name);

	template <typename T>
	void DeleteComponent()
	{
		GetSpace()->DeleteComponent<T>(m_objID);
	}

	operator bool() const { return m_objID; }
	operator GameObject_ID() { return m_objID; }

	// These are pointers, not handles. They probably won't last long.
	std::vector<meta::Any> GetComponentPointersMeta();

	bool IsValid() const;

	static GameObject_ID ConstructID(int id, GameSpaceIndex spaceIndex);

	// Sets the space that GameObjects will be deserialized into.
	// WARNING: THIS FUNCTION IS NOT THREADSAFE
	static void SetDeserializeSpace(GameSpaceIndex index);
	rapidjson::Value SerializeObject(rapidjson::Document::AllocatorType& allocator) const;
	void GameObject::DeserializeObject(rapidjson::Value& jsonValue);

	static GameObject FindByName(const char *name);
	static GameObject FindByTag(const char * tagStr);

private:
	union
	{
		struct
		{
			int m_id     : 24;
			int m_space  : 8;
		};
		GameObject_ID m_objID;
	};

	static rapidjson::Value GameObjectSerialize(const void *gameObjectPtr, rapidjson::Document::AllocatorType& allocator);
	static void GameObjectDeserializeAssign(void *gameObjectPtr, rapidjson::Value& jsonValue);

	// Returns a pointer. This is dangerous and probably bad because it's not a handle. 
	// Function created because Luabridge likes pointers, not component handles.
	template <typename T>
	T *GetComponentPointer()
	{
		return GetComponent<T>().Get();
	}

	META_REGISTER(GameObject)
	{
		META_DefineMember(GameObject, m_objID, "id");

		META_DefineFunction(GameObject, Duplicate, "Duplicate");
		META_DefineFunction(GameObject, Delete,       "Delete");

		META_DefineFunction(GameObject, DeleteComponent<RigidBodyComponent>,          "DeleteRigidBody");
		META_DefineFunction(GameObject, DeleteComponent<StaticCollider2DComponent>,   "DeleteStaticCollider");
		META_DefineFunction(GameObject, DeleteComponent<DynamicCollider2DComponent>,  "DeleteDynamicCollider");
		META_DefineFunction(GameObject, DeleteComponent<SpriteComponent>,             "DeleteSprite");
		META_DefineFunction(GameObject, DeleteComponent<Camera>,					            "DeleteCamera");


		META_DefineFunction(GameObject, GetComponentPointer<ObjectInfo>,                 "GetObjectInfo");
		META_DefineFunction(GameObject, GetComponentPointer<TransformComponent>,         "GetTransform");
		META_DefineFunction(GameObject, GetComponentPointer<RigidBodyComponent>,         "GetRigidBody");
		META_DefineFunction(GameObject, GetComponentPointer<StaticCollider2DComponent>,  "GetStaticCollider");
		META_DefineFunction(GameObject, GetComponentPointer<DynamicCollider2DComponent>, "GetDynamicCollider");
		META_DefineFunction(GameObject, GetComponentPointer<SpriteComponent>,            "GetSprite");
		META_DefineFunction(GameObject, GetComponentPointer<Camera>,                     "GetCamera");

		META_DefineFunction(GameObject, GetName, "GetName");
		META_DefineFunction(GameObject, SetName, "SetName");

		// TODO: Make this use the meta function.
		luabridge::getGlobalNamespace(GetGlobalLuaState()).beginClass<GameObject>("GameObject").addStaticFunction("FindByName", FindByName).endClass();
		luabridge::getGlobalNamespace(GetGlobalLuaState()).beginClass<GameObject>("GameObject").addStaticFunction("FindByTag", FindByTag).endClass();
	}
};

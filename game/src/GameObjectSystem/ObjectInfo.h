/*
FILE: ObjectInfo.h
PRIMARY AUTHOR: Sweet

Copyright � 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Universal.h"
#include "Component.h"

#include <map>
#include <string>

typedef unsigned ObjectTag;

class ObjectInfo
{
public:
	std::string m_name;
	GameObject_ID m_id;
	std::map<std::size_t, std::string> m_tags;
	
	// If false, level saves won't save this object.
	bool m_savesWithLevel = true;
	bool m_destroyed = false;
	bool m_active = true;

	ObjectInfo() : m_name(""), m_id(INVALID_GAMEOBJECT_ID) {}

	ObjectInfo(GameObject_ID id) : m_name(""), m_id(id) {}

	ObjectInfo(GameObject_ID id, const char *name) : m_name(name), m_id(id) {}

	ObjectInfo(GameObject_ID id, ObjectInfo& rhs) : m_name(rhs.m_name), m_id(id), m_tags(rhs.m_tags) {}

	void AddTag(const char *name) { m_tags.emplace(hash(name), name); }

	bool HasTag(const char *tag)
	{
		// Look for the tag in the map.
		for (auto& tagPair : m_tags)
		{
			if (tagPair.second == tag)
			{
				return true;
			}
		}

		// If we got here we couldn't find it.
		return false;
	}

private:
	// For meta.
	static void SetObjectID(void *objectInfoPtr, GameObject_ID id)
	{
		ObjectInfo& objectInfo = *reinterpret_cast<ObjectInfo *>(objectInfoPtr);

		objectInfo.m_id = id;
	}


	// Making the tag map deserializable.
	typedef std::map<std::size_t, std::string> TagMap;

	static rapidjson::Value TagMapSerializeFunction(const void *tagMapPtr, rapidjson::Document::AllocatorType& allocator)
	{
		const TagMap& tagMap = *reinterpret_cast<const TagMap *>(tagMapPtr);

		rapidjson::Value jsonTags;
		jsonTags.SetObject();

		for (auto& tagPair : tagMap)
		{
			rapidjson::Value key;
			key.SetInt64(tagPair.first);

			rapidjson::Value val;
			val.SetString(tagPair.second.c_str(), allocator);

			// val then key because val is a string.
			jsonTags.AddMember(val, key, allocator);
		}

		return jsonTags;
	}

	static void TagMapDeserializeAssign(void *tagMapPtr, rapidjson::Value& jsonEngine)
	{
		TagMap& tagMap = *reinterpret_cast<TagMap *>(tagMapPtr);

		for (auto& jsonPair : jsonEngine.GetObject())
		{
			size_t key = jsonPair.value.GetInt64();
			std::string val = jsonPair.name.GetString();

			tagMap.insert(std::make_pair(key, val));
		}
	}


	META_REGISTER(ObjectInfo)
	{
		META_DefineType(TagMap);
		META_DefineSerializeFunction(TagMap, TagMapSerializeFunction);
		META_DefineDeserializeAssignFunction(TagMap, TagMapDeserializeAssign);

		META_DefineMember(ObjectInfo, m_savesWithLevel, "savesWithLevel");

		META_DefineMember(ObjectInfo, m_tags, "tags");

		META_DefineMember(ObjectInfo, ObjectInfo::m_name, "name");
		META_DefineMember(ObjectInfo, m_id, "id");

		META_DefineMember(ObjectInfo, m_destroyed, "destroyed");
		META_DefineMember(ObjectInfo, m_active, "active");

		META_DefineSetGameObjectIDFunction(ObjectInfo, SetObjectID);

	}
};





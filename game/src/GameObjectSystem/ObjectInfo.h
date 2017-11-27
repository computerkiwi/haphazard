/*
FILE: ObjectInfo.h
PRIMARY AUTHOR: Sweet

Copyright © 2017 DigiPen (USA) Corporation.
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

	ObjectInfo() : m_name(""), m_id(INVALID_GAMEOBJECT_ID) {}

	ObjectInfo(GameObject_ID id) : m_name(""), m_id(id) {}

	ObjectInfo(GameObject_ID id, const char *name) : m_name(name), m_id(id) {}

	ObjectInfo(GameObject_ID id, ObjectInfo& rhs) : m_name(rhs.m_name), m_id(id), m_tags(rhs.m_tags) {}


	void AddTag(const char *name) { m_tags.emplace(hash(name), name); }

	META_REGISTER(ObjectInfo)
	{
		META_DefineMember(ObjectInfo, ObjectInfo::m_name, "name");
		META_DefineMember(ObjectInfo, m_id, "id");
	}
};





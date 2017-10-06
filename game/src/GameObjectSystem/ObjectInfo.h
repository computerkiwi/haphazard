/*
FILE: ObjectInfo.h
PRIMARY AUTHOR: Sweet

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once


#include "Component.h"

#include <vector>

typedef unsigned ObjectTag;

class ObjectInfo
{
public:
	const char *m_name;
	GameObject_ID m_id;
	std::vector<ObjectTag> m_tags;


	ObjectInfo(GameObject_ID id) : m_name(""), m_id(id) {}

	ObjectInfo(GameObject_ID id, const char *name) : m_name(name), m_id(id) {}

	void AddTag(ObjectTag tag) { m_tags.push_back(tag); }

};





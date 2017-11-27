/*
FILE: HierarchyComponent.h
PRIMARY AUTHOR: Sweet

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include <vector>
#include "GameObject.h"

class HierarchyComponent
{
public:
	std::vector<GameObject> m_children;

	HierarchyComponent();
	HierarchyComponent(GameObject first_child);
	void Add(GameObject child);
	void Remove(GameObject child);

	std::vector<GameObject>& GetList();

	GameObject Get(int index) const;
	GameObject operator[](int index) const;

	META_REGISTER(HierarchyComponent)
	{
	}

};

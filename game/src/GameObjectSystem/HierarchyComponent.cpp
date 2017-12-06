/*
FILE: HierarchyComponent.cpp
PRIMARY AUTHOR: Noah Sweet

Copyright (c) 2017 DigiPen (USA) Corporation.
*/

#include "GameSpace.h"
#include "Component.h"
#include "HierarchyComponent.h"
#include "TransformComponent.h"


HierarchyComponent::HierarchyComponent()
{
}


HierarchyComponent::HierarchyComponent(GameObject first_child) : m_children()
{
	m_children.emplace_back(first_child);
}


void HierarchyComponent::Add(GameObject child)
{
	m_children.emplace_back(child);
}


void HierarchyComponent::Remove(GameObject child)
{
	for (size_t i = 0; i < m_children.size(); i++)
	{
		if (m_children[i] == child)
		{
			m_children[i].GetComponent<TransformComponent>()->SetParent(0);

			m_children.erase(m_children.begin() + i);
		}
	}
}


std::vector<GameObject>& HierarchyComponent::GetList()
{
	return m_children;
}


GameObject HierarchyComponent::Get(int index) const
{
	if (index < m_children.size())
	{
		return m_children[index];
	}
	else
	{
		return 0;
	}
}


GameObject HierarchyComponent::operator[](int index) const
{
	return Get(index);
}

/*
FILE: QuadTree.cpp
PRIMARY AUTHOR: Sweet

Contents the implementation for the QuadTrees

Copyright � 2017 DigiPen (USA) Corporation.
*/
#include "Quadtree.h"
#include "../../GameObjectSystem/TransformComponent.h"
#include "GameObjectSystem\GameObject.h"
#include <array>

constexpr std::size_t Max_Leaves(int depth)
{
	std::size_t total = 1;
	for (auto i = 1; i <= depth; ++i)
	{
		auto power = 1;
		for (auto j = 1; j <= i; ++j)
		{
			power *= 4;
		}
		total += power;
	}
	return total;
}



#if 0

// Figure out how much is in quadtree at each depth
Array<QuadTree, Max_Leaves(MAX_DEPTH)> QuadTree__Allocator__;

void QuadTree::Reset()
{
	QuadTree__Allocator__.m_size = 0;
}


QuadTree::QuadTree() : QuadTree(glm::vec2(), glm::vec2(), 0)
{
}


QuadTree::QuadTree(glm::vec2 & minPoint, glm::vec2 & maxPoint, int depth)
	: m_minPoint(minPoint), m_maxPoint(maxPoint), m_center((minPoint.x + maxPoint.x) / 2, (minPoint.y + maxPoint.y) / 2),
	m_depth(depth)
{
}


QuadTree::QuadTree(glm::vec2 && minPoint, glm::vec2 && maxPoint, int depth)
	: m_minPoint(minPoint), m_maxPoint(maxPoint), m_center((minPoint.x + maxPoint.x) / 2, (minPoint.y + maxPoint.y) / 2),
	m_depth(depth)
{
}


QuadTree::QuadTree(float x_min, float y_min, float x_max, float y_max, int depth) 
	: m_minPoint(x_min, y_min), m_maxPoint(x_max, y_max), m_center((m_minPoint.x + m_maxPoint.x) / 2, (m_minPoint.y + m_maxPoint.y) / 2),
	m_depth(depth)
{
}


QuadTree::QuadTree(float x_min, float y_min, glm::vec2 & maxPoint, int depth)
	: m_minPoint(x_min, y_min), m_maxPoint(maxPoint), m_center((m_minPoint.x + m_maxPoint.x) / 2, (m_minPoint.y + m_maxPoint.y) / 2),
	m_depth(depth)
{
}


QuadTree::QuadTree(float x_min, float y_min, glm::vec2 && maxPoint, int depth)
	: m_minPoint(x_min, y_min), m_maxPoint(maxPoint), m_center((m_minPoint.x + m_maxPoint.x) / 2, (m_minPoint.y + m_maxPoint.y) / 2),
	m_depth(depth)
{
}


QuadTree::~QuadTree()
{
	Clear();
}


void QuadTree::Clear()
{
	if (m_children.m_size)
	{
		for (auto i = 0; i < m_children.m_size; ++i)
		{
			// delete m_children[i]; -- Hot swapping with big ass array
			m_children[i] = nullptr;
		}

		m_children.m_size = 0;
	}
}


const Array<GameObject *, max_objects> & QuadTree::GetObjectList() const
{
	return m_objects;
}


const Array<QuadTree *, 4> & QuadTree::GetChildren() const
{
	return m_children;
}


void QuadTree::AddChildern()
{
	if (m_depth > MAX_DEPTH)
	{
		return;
	}
	++m_depth;

	// c0
	m_children[0] = QuadTree__Allocator__.Push(QuadTree(glm::vec2(m_minPoint.x, m_center.y), glm::vec2(m_center.x, m_maxPoint.y), m_depth));

	// c1
	m_children[1] = QuadTree__Allocator__.Push(QuadTree(m_minPoint, m_center, m_depth));

	// c2
	m_children[2] = QuadTree__Allocator__.Push(QuadTree(glm::vec2(m_center.x, m_minPoint.y), glm::vec2(m_maxPoint), m_depth));

	// c3
	m_children[3] = QuadTree__Allocator__.Push(QuadTree(m_center, m_maxPoint, m_depth));

	m_children.m_size = 4;

	for (auto i = 0; i < m_objects.m_size; ++i)
	{
		GameObject * object = m_objects[i];
		if (object)
		{
			FileToChildren(object, object->GetComponent<TransformComponent>().GetPosition());
			m_objects[i] = nullptr;
		}
	}
	m_objects.m_size = 0;
}


void QuadTree::DestroyChildren()
{
	int newNumObjects = CollectObjects(m_objects);

	Clear();

	m_children.m_size = 0;
	m_objects.m_size = newNumObjects;
}


int QuadTree::CollectObjects(Array<GameObject *, max_objects> & list)
{
	auto newNum = 1;
	if (m_children.m_size)
	{
		for (auto i = 0; i < m_children.m_size; ++i)
		{
			newNum += m_children[i]->CollectObjects(list);
		}
	}
	else
	{
		for (auto i = 0; i < m_objects.m_size; ++i)
		{
			list.Push(m_objects[i]);
		}
	}
	return newNum;
}


void QuadTree::AddObject(GameObject * object, const glm::vec2 & pos)
{
	if (m_children.m_size == 0 && m_depth < MAX_DEPTH && m_objects.m_size >= max_objects - 1)
	{
		AddChildern();
	}

	if (m_depth >= MAX_DEPTH)
		return;

	if (m_children.m_size)
	{
		FileToChildren(object, pos);
	}
	else
	{
		AddToEnd(object);
	}
}


void QuadTree::RemoveObject(GameObject * object, glm::vec2 & pos)
{
	if (m_children.m_size && m_objects.m_size < 1)
	{
		DestroyChildren();
	}

	if (m_children.m_size)
	{
		FileToChildren(object, pos);
	}
	else
	{
		auto posInVect = FindIndexOfObject(object);
		if (posInVect >= 0)
		{
			m_objects[posInVect] = nullptr;

			--m_objects.m_size;
		}
	}
}


void QuadTree::MoveObject(GameObject * object, glm::vec2 & oldPos)
{
	RemoveObject(object, oldPos);
	AddObject(object, oldPos);
}


int QuadTree::FindIndexOfObject(GameObject * object)
{
	for (auto i = 0; i < m_objects.m_size; ++i)
	{
		if (m_objects[i] == object)
			return i;
	}

	return -1;
}


void QuadTree::AddToEnd(GameObject * object)
{
	m_objects[m_objects.m_size++] = object;
}


void QuadTree::FileToChildren(GameObject * object, const glm::vec2 & pos)
{
	glm::vec2 objectSize = object->GetComponent<TransformComponent>().GetScale(); //!?!? Replace with HitBox
	objectSize.x /= 2;
	objectSize.y /= 2;

	float radius = sqrt(objectSize.x * objectSize.x + objectSize.y * objectSize.y);

	glm::vec2 top(pos.x, pos.y + radius);
	glm::vec2 bottom(pos.x, pos.y - radius);
	glm::vec2 left(pos.x - radius, pos.y);
	glm::vec2 right(pos.x + radius, pos.y);

	// Right
	if (left.x > m_center.x)
	{
		// Up
		if (bottom.y > m_center.y)
		{
			m_children[3]->AddObject(object, pos);
		}

		// Down
		else if (top.y < m_center.y)
		{
			m_children[2]->AddObject(object, pos);
		}

		// In Both Planes
		else
		{
			m_children[3]->AddObject(object, pos);
			m_children[2]->AddObject(object, pos);
		}
	}

	// Left
	else if (right.x < m_center.x)
	{
		// Up
		if (bottom.y > m_center.y)
		{
			m_children[0]->AddObject(object, pos);
		}

		// Down
		else if (top.y < m_center.y)
		{
			m_children[1]->AddObject(object, pos);
		}

		// In Both Planes
		else
		{
			m_children[0]->AddObject(object, pos);
			m_children[1]->AddObject(object, pos);
		}
	}

	// Edge Cases
	else
	{
		// On the y-axis
		if (right.x > m_center.x && left.x < m_center.x)
		{
			// Origin
			if (top.y > m_center.y && bottom.y < m_center.y)
			{
				m_children[0]->AddObject(object, pos);
				m_children[1]->AddObject(object, pos);
				m_children[2]->AddObject(object, pos);
				m_children[3]->AddObject(object, pos);
			}

			// Top Half
			else if (bottom.y > m_center.y)
			{
				m_children[0]->AddObject(object, pos);
				m_children[3]->AddObject(object, pos);
			}

			// Bottom Half
			else if (top.y < m_center.y)
			{
				m_children[1]->AddObject(object, pos);
				m_children[2]->AddObject(object, pos);
			}

			// Edge Cases
			else
			{
				m_children[0]->AddObject(object, pos);
				m_children[1]->AddObject(object, pos);
				m_children[2]->AddObject(object, pos);
				m_children[3]->AddObject(object, pos);
			}
		}

		// On the x-axis
		else if (bottom.y < m_center.y && top.y > m_center.y)
		{
			// Left Half
			if (right.x < m_center.x)
			{
				m_children[0]->AddObject(object, pos);
				m_children[1]->AddObject(object, pos);
			}

			// Bottom Half
			else if (left.x > m_center.x)
			{
				m_children[2]->AddObject(object, pos);
				m_children[3]->AddObject(object, pos);
			}

			// Edge Cases
			else
			{
				m_children[0]->AddObject(object, pos);
				m_children[1]->AddObject(object, pos);
				m_children[2]->AddObject(object, pos);
				m_children[3]->AddObject(object, pos);
			}
		}
	}
}
#endif

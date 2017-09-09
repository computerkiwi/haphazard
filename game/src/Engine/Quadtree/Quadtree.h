/*
FILE: QuadTree.h
PRIMARY AUTHOR: Sweet

Definition for QuadTrees

Copyright � 2017 DigiPen (USA) Corporation.
*/
#pragma once
#if 0
#include <glm/detail/type_vec2.hpp>

#include "../../Util/DataStructures/Array/Array.h"

#define MAX_DEPTH 5
constexpr const std::size_t max_objects = 15;


class QuadTree
{
	glm::vec2 m_minPoint;
	glm::vec2 m_maxPoint;
	glm::vec2 m_center;

	Array<GameObject *, max_objects> m_objects; 
	Array<QuadTree *, 4> m_children;

	int m_depth = 0;
public:
	static void Reset();

	QuadTree();

	QuadTree(glm::vec2 & minPoint, glm::vec2 & maxPoint, int depth = 0);

	QuadTree(glm::vec2 && minPoint, glm::vec2 && maxPoint, int depth = 0);

	QuadTree(float x_min, float y_min, glm::vec2 & maxPoint, int depth = 0);
	QuadTree(float x_min, float y_min, glm::vec2 && maxPoint, int depth = 0);
	QuadTree(float x_min, float y_min, float x_max, float y_max, int depth = 0);

	~QuadTree();

	void Clear();

	const Array<GameObject *, max_objects> & GetObjectList() const;
	const Array<QuadTree *, 4> & GetChildren() const;

	void AddChildern();

	void DestroyChildren();

	int CollectObjects(Array<GameObject *, max_objects> & list);

	void AddObject(GameObject * object, const glm::vec2 & pos);

	void RemoveObject(GameObject * object, glm::vec2 & pos);

	void MoveObject(GameObject * object, glm::vec2 & oldPos);

	int FindIndexOfObject(GameObject * object);

	void AddToEnd(GameObject * object);

	void FileToChildren(GameObject * object, const glm::vec2 & pos);
};

#endif





/*
FILE: GameObject.h
PRIMARY AUTHOR: Kieran

Copyright � 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Engine\Engine.h"
#include "GameObject.h"

extern Engine *engine;


GameObject::GameObject(GameObject_ID id, GameSpaceIndex gameSpace) : m_objID(id & (gameSpace << EXTRACTION_SHIFT))
{
}


implicit GameObject::GameObject(GameObject_ID id) : m_objID(id)
{
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
	return m_space; // m_objID & 0xFF00000000000000;
}


void GameObject::SetSpace(GameSpaceIndex index)
{
	m_space = index;
}


GameObject_ID GameObject::Duplicate() const
{
	return GetSpace()->Duplicate(m_objID);
}


void GameObject::Delete()
{
	GetSpace()->Delete(m_objID);
	m_objID = 0;
}


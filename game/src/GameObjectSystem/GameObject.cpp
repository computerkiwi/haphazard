/*
FILE: GameObject.h
PRIMARY AUTHOR: Kieran

Copyright � 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Engine\Engine.h"
#include "GameObject.h"

extern Engine *engine;


GameObject::GameObject(int id, GameSpaceIndex gameSpace) : m_id(id), m_space(gameSpace)
{
}


implicit GameObject::GameObject(GameObject_ID id) : m_objID(id)
{
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
	return engine->GetSpace(m_space);
}


GameSpaceIndex GameObject::GetIndex() const
{
	return m_space;
}


void GameObject::SetSpace(GameSpaceIndex index)
{
	m_space = index;
}


GameObject_ID GameObject::Duplicate() const
{
	return GetSpace()->Duplicate(m_objID, GenerateID());
}


void GameObject::Delete()
{
	GetSpace()->Delete(m_objID);
	m_objID = 0;
}


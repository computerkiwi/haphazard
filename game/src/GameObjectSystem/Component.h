/*
FILE: Component.h
PRIMARY AUTHOR: Kieran

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "GameObject.h"

// Forward declaration.
class GameSpace;

template <typename T>
class ComponentHandle
{
public:
	ComponentHandle(GameObject_ID id, GameSpace *gameSpace, bool isValid = true);

	bool operator== (const ComponentHandle& other);

	bool operator!=(const ComponentHandle& other);

	T *operator->();

	T *Get();

	T& operator*();

	GameObject GetGameObject();

	template <typename ComponentType>
	ComponentHandle<ComponentType> GetSiblingComponent();

	// Returns true if this is a valid component.
	bool IsValid();

private:
	GameObject_ID m_objID;
	GameSpace *m_gameSpace;
	bool m_isValid;
};

#include "Component.inl"

/*
FILE: Component.h
PRIMARY AUTHOR: Kieran

Copyright � 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "GameObjectSystem\GameObject.h"

// Forward declaration.
class GameSpace;
#define EXTRACTION_SHIFT (8 * 3)

template <typename T>
class ComponentHandle
{
public:
	ComponentHandle(GameObject_ID id, bool isValid = true);
	ComponentHandle();

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

	// Returns true if this is a active component.
	bool IsActive();

private:
	GameObject_ID m_objID;
	bool m_isValid;
};

#include "Component.inl"

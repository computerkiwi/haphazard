/*
FILE: Component.inl
PRIMARY AUTHOR: Kieran

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

template <typename T>
ComponentHandle<T>::ComponentHandle(GameObject_ID id, GameSpace *gameSpace, bool isValid = true) : m_objID(id), m_gameSpace(gameSpace), m_isValid(isValid)
{
}

template <typename T>
bool ComponentHandle<T>::operator== (const ComponentHandle& other)
{
	return m_objID == other.m_objID;
}

template <typename T>
bool ComponentHandle<T>::operator!=(const ComponentHandle& other)
{
	return !(*this == other);
}

template <typename T>
T *ComponentHandle<T>::operator->()
{
	return m_gameSpace->GetInternalComponent<T>(m_objID);
}

template <typename T>
T *ComponentHandle<T>::Get()
{
	return m_gameSpace ? operator->() : nullptr;
}

template <typename T>
T& ComponentHandle<T>::operator*()
{
	return *m_gameSpace->GetInternalComponent<T>(m_objID);
}

template <typename T>
GameObject ComponentHandle<T>::GetGameObject()
{
	return GameObject(m_objID, m_gameSpace);
}

template <typename T>
template <typename ComponentType>
ComponentHandle<ComponentType> ComponentHandle<T>::GetSiblingComponent()
{
	// Make sure the component exists before we hand it off.
	if (m_gameSpace->GetInternalComponent<ComponentType>(m_objID) != nullptr)
	{
		return ComponentHandle<ComponentType>(m_objID, m_gameSpace);
	}
	else
	{
		return ComponentHandle<ComponentType>(0, nullptr, false);
	}
}

// Returns true if this is a valid component.
template <typename T>
bool ComponentHandle<T>::IsValid()
{
	return m_isValid;
}

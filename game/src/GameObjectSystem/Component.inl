/*
FILE: Component.inl
PRIMARY AUTHOR: Kieran

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

template <typename T>
ComponentHandle<T>::ComponentHandle(GameObject_ID id, bool isValid = true) : m_objID(id), m_isValid(isValid)
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
	return GameObject(m_objID).GetSpace()->GetInternalComponent<T>(m_objID);
}

template <typename T>
T *ComponentHandle<T>::Get()
{
	return GameObject(m_objID).GetSpace() ? operator->() : nullptr;
}

template <typename T>
T& ComponentHandle<T>::operator*()
{
	return *GameObject(m_objID).GetSpace()->GetInternalComponent<T>(m_objID);
}

template <typename T>
GameObject ComponentHandle<T>::GetGameObject()
{
	return GameObject(m_objID);
}

template <typename T>
template <typename ComponentType>
ComponentHandle<ComponentType> ComponentHandle<T>::GetSiblingComponent()
{
	// Make sure the component exists before we hand it off.
	if (GameObject(m_objID).GetSpace()->GetInternalComponent<ComponentType>(m_objID) != nullptr)
	{
		return ComponentHandle<ComponentType>(m_objID);
	}
	else
	{
		return ComponentHandle<ComponentType>(0, false);
	}
}

// Returns true if this is a valid component.
template <typename T>
bool ComponentHandle<T>::IsValid()
{
	return m_isValid;
}

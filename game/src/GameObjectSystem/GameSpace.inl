/*
FILE: GameSpace.h
PRIMARY AUTHOR: Kiera

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

//--------------------------------
// Standard ComponentMap template
//--------------------------------
template <typename T>
ComponentMap<T>::~ComponentMap()
{
}

template <typename T>
ComponentMap<T>::ComponentMap(GameSpace *space) : m_space(space)
{
}

// Returns nullptr if it's not found.
template <typename T>
T *ComponentMap<T>::get(GameObject_ID id)
{
	// Look for the object.
	auto iter = m_components.find(id);

	// If we couldn't find it, return nullptr. Else return pointer to it.
	if (iter == m_components.end())
	{
		return nullptr;
	}
	else
	{
		return &iter->second;
	}
}

template <typename T>
void ComponentMap<T>::Duplicate(GameObject_ID originalObject, GameObject_ID newObject)
{
	if (m_components.find(originalObject) != m_components.end())
	{
		m_components.emplace(newObject, T(m_components.find(originalObject)->second));
	}
}

template <typename T>
void ComponentMap<T>::Duplicate(GameObject sourceObject)
{
	if (sourceObject.GetComponent<T>().IsValid())
	{
		m_components.emplace(sourceObject, *sourceObject.GetComponent<T>());
	}
}


template <typename T>
void ComponentMap<T>::Delete(GameObject_ID object)
{
	if (m_components.find(object) != m_components.end())
	{
		m_components.erase(object);
	}
}


template <typename T>
meta::Any ComponentMap<T>::GetComponentPointerMeta(GameObject_ID object)
{
	return meta::Any(this->get(object));
}
	
template <typename T>
typename ComponentMap<T>::iterator ComponentMap<T>::begin()
{
	return iterator(m_components.begin(), m_space);
}

template <typename T>
typename ComponentMap<T>::iterator ComponentMap<T>::end()
{
	return iterator(m_components.end(), m_space);
}

template <typename T>
template <typename... Args>
void ComponentMap<T>::emplaceComponent(GameObject_ID id, Args&&... args)
{
	T obj(std::forward<Args>(args)...);

	m_components.insert_or_assign(id, obj);
}

template <typename T>
void ComponentMap<T>::DeleteComponent(GameObject_ID id)
{
	m_components.erase(id);
}

//-----------------------------------------
// Standard ComponentMap template iterator
//-----------------------------------------

template <typename T>
ComponentMap<T>::iterator::iterator(typename std::unordered_map<GameObject_ID, T>::iterator iterator, GameSpace *space) : m_iterator(iterator), m_space(space)
{
}

template <typename T>
typename ComponentMap<T>::iterator& ComponentMap<T>::iterator::operator++()
{
	++m_iterator;

	return *this;
}

template <typename T>
typename ComponentMap<T>::iterator ComponentMap<T>::iterator::operator++(int)
{
	iterator temp = *this;

	++m_iterator;

	return temp;
}

template <typename T>
bool ComponentMap<T>::iterator::operator==(const iterator& other) const
{
	return (m_iterator == other.m_iterator) && (m_space == other.m_space);
}

template <typename T>
bool ComponentMap<T>::iterator::operator!=(const iterator& other) const
{
	return !(*this == other);
}

template <typename T>
ComponentHandle<T> ComponentMap<T>::iterator::operator*()
{
	return ComponentHandle<T>(m_iterator->first, m_space);
}

template <typename T>
ComponentHandle<T> ComponentMap<T>::iterator::operator->()
{
	return ComponentHandle<T>(m_iterator->first, m_space);
}

//------------------------------
// GameSpace Template Functions
//------------------------------

template <typename T>
void GameSpace::RegisterComponentType()
{
	Logging::Log(Logging::CORE, Logging::MEDIUM_PRIORITY, "Gamespace ", this, " registering component type ", typeid(T).name());
	m_componentMaps.emplace(GetComponentType<T>::func(), new ComponentMap<T>(this));

	std::cout << "Registering component type" << GetComponentType<T>::func() << std::endl;
}

// Returns a component HANDLE.
template <typename T>
ComponentHandle<T> GameSpace::GetComponent(GameObject_ID id)
{
	return ComponentHandle<T>(id, this);
}

template <typename T>
ComponentMap<T> *GameSpace::GetComponentMap()
{
	ComponentMapBase *baseMap = m_componentMaps.at(GetComponentType<T>::func());
	return static_cast<ComponentMap<T> *>(baseMap);
}

template <typename T>
void GameSpace::DeleteComponent(GameObject_ID id)
{
	reinterpret_cast<ComponentMap<T> *>(m_componentMaps.at(GetComponentType<T>::func()))->DeleteComponent(id);
}

template <typename T>
T *GameSpace::GetInternalComponent(GameObject_ID id)
{
	// TODO[Kiera]: Cast individual components instead of the maps.

	ComponentMapBase *baseMap = m_componentMaps.at(GetComponentType<T>::func());
	ComponentMap<T> *compMap = static_cast<ComponentMap<T> *>(baseMap);

	return compMap->get(id);
}

template <typename T, typename... Args>
void GameSpace::EmplaceComponent(GameObject_ID id, Args&&... args)
{
	// TODO[Kiera]: Cast individual components instead of the maps.

	ComponentMapBase *baseMap = m_componentMaps.at(GetComponentType<T>::func());
	ComponentMap<T> *compMap = static_cast<ComponentMap<T> *>(baseMap);

	compMap->emplaceComponent(id, std::forward<Args>(args)...);
}

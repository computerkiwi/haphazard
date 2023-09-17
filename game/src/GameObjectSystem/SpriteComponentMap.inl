/*
FILE: SpriteComponentMap.inl
PRIMARY AUTHOR: Kiera

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "GameSpace.h"
#include "graphics\SpriteComponent.h"

struct SpriteReference
{
	GameObject_ID id;
	SpriteComponent *ptr;
};

template <>
class ComponentMap<SpriteComponent> : public ComponentMapBase
{
public:
	virtual ~ComponentMap();

	ComponentMap(GameSpace *space);

	// Returns nullptr if it's not found.
	SpriteComponent *get(GameObject_ID id);

	void Duplicate(GameObject_ID originalObject, GameObject_ID newObject) override;

	void Duplicate(GameObject sourceObject) override;

	void Delete(GameObject_ID object) override;

	virtual meta::Any GetComponentPointerMeta(GameObject_ID object);

	class iterator
	{
	public:
		iterator(std::vector<SpriteReference>::iterator iterator);

		iterator& operator++();

		iterator operator++(int);

		bool operator==(const iterator& other) const;

		bool operator!=(const iterator& other) const;

		ComponentHandle<SpriteComponent> operator*();

		ComponentHandle<SpriteComponent> operator->();

	private:
		std::vector<SpriteReference>::iterator m_iterator;
	};

	iterator begin();

	iterator end();

	template <typename... Args>
	void emplaceComponent(GameObject_ID id, Args&&... args)
	{
		SpriteComponent obj(std::forward<Args>(args)...);

		m_components.insert_or_assign(id, obj);
		SpriteReference sprRef;
		sprRef.id = id;
		sprRef.ptr = &m_components[id];
		m_references.push_back(sprRef);
		m_refsDirty = true;
	}

	void DeleteComponent(GameObject_ID id);

	friend void StaticUpdateSpaceIndex<ObjectInfo>(ComponentMap<ObjectInfo>& map, GameSpaceIndex index);

	virtual void UpdateSpaceIndex(GameSpaceIndex index)
	{
		StaticUpdateSpaceIndex<SpriteComponent>(*this, index);
	}

	static void ResortAll()
	{
		for (auto map : spriteMaps)
		{
			map->m_refsDirty = true;
		}
	}

private:
	std::vector<SpriteReference> m_references;
	bool m_refsDirty;
	std::unordered_map<GameObject_ID, SpriteComponent> m_components;
	GameSpace *m_space;

	virtual ComponentMapBase *NewDuplicateMap() override
	{
		return new ComponentMap<SpriteComponent>(*this);
	}

	static std::vector<ComponentMap<SpriteComponent> *> spriteMaps;

	void ResortReferences()
	{
		// Sort based on z layer.
		std::sort(m_references.begin(), m_references.end(), [](const SpriteReference& a, const SpriteReference& b)
		{
			GameObject aObj(a.id);
			GameObject bObj(b.id);

			if (!aObj.IsValid())
			{
				return true;
			}
			if (!bObj.IsValid())
			{
				return false;
			}

			return aObj.GetComponent<TransformComponent>()->GetZLayer() < bObj.GetComponent<TransformComponent>()->GetZLayer();
		});
	}
};

//--------------------------------
// SpriteComponent ComponentMap template
//--------------------------------
inline ComponentMap<SpriteComponent>::~ComponentMap()
{
	spriteMaps.erase(std::find(spriteMaps.begin(), spriteMaps.end(), this));
}

inline ComponentMap<SpriteComponent>::ComponentMap(GameSpace *space) : m_space(space), m_refsDirty(true)
{
	spriteMaps.push_back(this);
}

// Returns nullptr if it's not found.
inline SpriteComponent *ComponentMap<SpriteComponent>::get(GameObject_ID id)
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

inline void ComponentMap<SpriteComponent>::Duplicate(GameObject_ID originalObject, GameObject_ID newObject)
{
	if (m_components.find(originalObject) != m_components.end())
	{
		emplaceComponent(newObject, m_components.find(originalObject)->second);
	}
}

inline void ComponentMap<SpriteComponent>::Duplicate(GameObject sourceObject)
{
	if (sourceObject.GetComponent<SpriteComponent>().IsValid())
	{
		emplaceComponent(sourceObject, m_components.find(sourceObject)->second);
	}
}


inline void ComponentMap<SpriteComponent>::Delete(GameObject_ID object)
{
	if (m_components.find(object) != m_components.end())
	{
		m_components.erase(object);

		std::swap(*std::find_if(m_references.begin(), m_references.end(), [object](const SpriteReference& ref) {return ref.id == object; }), m_references.back());
		m_references.pop_back();

		m_refsDirty = true;
	}
}


inline meta::Any ComponentMap<SpriteComponent>::GetComponentPointerMeta(GameObject_ID object)
{
	return meta::Any(this->get(object));
}

inline typename ComponentMap<SpriteComponent>::iterator ComponentMap<SpriteComponent>::begin()
{
	if (m_refsDirty)
	{
		ResortReferences();
		m_refsDirty = false;
	}
	return iterator(m_references.begin());
}

inline typename ComponentMap<SpriteComponent>::iterator ComponentMap<SpriteComponent>::end()
{
	return iterator(m_references.end());
}

inline void ComponentMap<SpriteComponent>::DeleteComponent(GameObject_ID id)
{
	Delete(id);
}

//-----------------------------------------
// Standard ComponentMap template iterator
//-----------------------------------------

inline ComponentMap<SpriteComponent>::iterator::iterator(typename std::vector<SpriteReference>::iterator iterator) : m_iterator(iterator)
{
}

inline ComponentMap<SpriteComponent>::iterator& ComponentMap<SpriteComponent>::iterator::operator++()
{
	++m_iterator;

	return *this;
}

inline ComponentMap<SpriteComponent>::iterator ComponentMap<SpriteComponent>::iterator::operator++(int)
{
	iterator temp = *this;

	++m_iterator;

	return temp;
}

inline bool ComponentMap<SpriteComponent>::iterator::operator==(const iterator& other) const
{
	return (m_iterator == other.m_iterator);
}

inline bool ComponentMap<SpriteComponent>::iterator::operator!=(const iterator& other) const
{
	return !(*this == other);
}

inline ComponentHandle<SpriteComponent> ComponentMap<SpriteComponent>::iterator::operator*()
{
	return ComponentHandle<SpriteComponent>(m_iterator->id);
}

inline ComponentHandle<SpriteComponent> ComponentMap<SpriteComponent>::iterator::operator->()
{
	return ComponentHandle<SpriteComponent>(m_iterator->id);
}

/*
FILE: UnorderedIterable.inl
PRIMARY AUTHOR: Kieran

Copyright (c) 2018 DigiPen (USA) Corporation.
*/

#include "UnorderedIterable.h"

// TODO: Make it so these won't leak into other scopes when included.
#ifndef INITIAL_SIZE
#define INITIAL_SIZE 2
#endif INITIAL_SIZE

#ifndef GROWTH_FACTOR
#define GROWTH_FACTOR 1.5f
#endif GROWTH_FACTOR

//////////////////////////////////////////////
// Constructors, etc.

template<typename T>
inline UnorderedIterable<T>::UnorderedIterable() : m_data(new Element[INITIAL_SIZE]), m_size(0), m_capacity(INITIAL_SIZE), m_furthestIndex(-1), m_freeListHead(m_data)
{
	// Setup the initial free list.
	InitElementList(0, m_capacity);
}

template<typename T>
inline UnorderedIterable<T>::~UnorderedIterable()
{
	delete[] m_data;
}

//////////////////////////////////////////////
// Access

template<typename T>
inline T & UnorderedIterable<T>::at(ID id)
{
	if (id < 0 || id > m_furthestIndex || !m_data[id].HasValue())
	{
		throw std::out_of_range("UnorderedIterable.at: Invalid ID!");
	}

	return (*this)[id];
}

template<typename T>
inline T & UnorderedIterable<T>::operator[](ID id)
{
	return m_data[id].value;
}

//////////////////////////////////////////////
// Iterators

template <typename T>
typename UnorderedIterable<T>::iterator& UnorderedIterable<T>::iterator::operator++()
{
	do
	{
		++m_index;

	} while (m_index < m_iterable->m_furthestIndex && !m_iterable->m_data[m_index].HasValue());

	return *this;
}

template <typename T>
T& UnorderedIterable<T>::iterator::operator*()
{
	return m_iterable->m_data[m_index].value;
}

template<typename T>
inline bool UnorderedIterable<T>::iterator::operator==(const iterator & other) const
{
	return (m_iterable == other.m_iterable && m_index == other.m_index);
}

template<typename T>
inline bool UnorderedIterable<T>::iterator::operator!=(const iterator & other) const
{
	return !(*this == other);
}

template<typename T>
inline UnorderedIterable<T>::iterator::iterator(UnorderedIterable & iterable, ID index) : m_iterable(&iterable), m_index(index)
{
}

template<typename T>
inline typename UnorderedIterable<T>::iterator UnorderedIterable<T>::begin()
{
	iterator iter = iterator(*this, 0);
	// The front index might not actually be 0.
	if (!m_data[0].HasValue())
	{
		++iter;
	}

	return iter;
}

template<typename T>
inline typename  UnorderedIterable<T>::iterator UnorderedIterable<T>::end()
{
	return iterator(*this, m_furthestIndex + 1);
}

//////////////////////////////////////////////
// Capacity

template<typename T>
inline bool UnorderedIterable<T>::empty() const
{
	return m_size == 0;
}

template<typename T>
inline size_t UnorderedIterable<T>::size() const
{
	return m_size;
}

template<typename T>
inline size_t UnorderedIterable<T>::capacity() const
{
	return m_capacity;
}

//////////////////////////////////////////////
// Modification

template<typename T>
inline void UnorderedIterable<T>::clear()
{
	m_size = 0;
	m_freeListHead = InitElementList(0, m_capacity);
	m_furthestIndex = 0;
}

template<typename T>
inline typename UnorderedIterable<T>::ID UnorderedIterable<T>::insert(const T & val)
{
	if (m_freeListHead == nullptr)
	{
		// Set up a new array.
		size_t newCapacity = m_capacity * GROWTH_FACTOR;
		Element *newArray = new Element[newCapacity];

		// Copy elements over.
		for (size_t i = 0; i < m_capacity; ++i)
		{
			newArray[i] = m_data[i];
		}

		// Finalize the new array.
		delete[] m_data;
		m_data = newArray;

		// Hook up the new free list.
		m_freeListHead = InitElementList(m_capacity, newCapacity);
		m_capacity = newCapacity;
	}

	// Pull off the next free element.
	Element *newElement = m_freeListHead;
	m_freeListHead = m_freeListHead->next;

	// Offset into the array is the index.
	ID index = newElement - m_data;
	if (static_cast<long>(index) > m_furthestIndex)
	{
		m_furthestIndex = index;
	}

	// Set the actual data.
	newElement->next = nullptr;
	newElement->value = val;
	++m_size;

	return index;
}

template<typename T>
inline void UnorderedIterable<T>::remove(ID id)
{
	Element *removedElement = &m_data[id];

	// Add it to the free list.
	removedElement->next = m_freeListHead;
	m_freeListHead = removedElement;
	--m_size;

	// Update the furthest index if we're removing it.
	if (m_furthestIndex == id)
	{
		// Look for the new furthest index.
		ID searchID = id - 1;
		while (!m_data[searchID].HasValue() && searchID > 0)
		{
			--searchID;
		}
		m_furthestIndex = searchID;
	}
	
}

//////////////////////////////////////////////
// Private Helpers

template<typename T>
inline typename UnorderedIterable<T>::Element *UnorderedIterable<T>::InitElementList(ID begin, ID end, Element * endPointer)
{
	// Setup the initial free list.
	// TODO: This should probably be its own function.
	for (ID i = begin; i < end - 1; ++i)
	{
		m_data[i].next = &m_data[i + 1];
	}
	m_data[end - 1].next = endPointer;

	return &m_data[begin];
}

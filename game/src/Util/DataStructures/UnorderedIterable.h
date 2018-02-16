/*
FILE: UnorderedIterable.h
PRIMARY AUTHOR: Kieran

Copyright (c) 2018 DigiPen (USA) Corporation.
*/
#pragma once

/*----------------------------------

UnorderedIterable is a simple data structure with the core goal of being fast
to iterate through while maintaining the indeces of elements in the structure.

Some things to note:

- Elements are stored in a sequential, cache-friendly way.
- Elements will maintain their ID as long as they stay in the structure.
- IDs are not globally unique. Once an element is removed from the structure its ID is free to be used again.

*-----------------------------------*/

#include <utility>

template <typename T>
class UnorderedIterable
{
public:

	// ID used to retrieve inserted elements.
	typedef size_t ID;

	//////////////////////////////////////////////
	// Constructors, etc.

	UnorderedIterable();
	~UnorderedIterable();
	UnorderedIterable(const UnorderedIterable& other);
	UnorderedIterable& operator=(const UnorderedIterable& other);

	//////////////////////////////////////////////
	// Access

	T& at(ID id);
	const T& at(ID id) const;

	bool has_element(ID id) const;

	T& operator[](ID id);
	const T& operator[](ID id) const;

	//////////////////////////////////////////////
	// Iterators

	class iterator
	{
	public:
		iterator& operator++();

		iterator operator++(int);

		T& operator*();

		T *operator->();

		bool operator==(const iterator& other) const;

		bool operator!=(const iterator& other) const;

	protected:
		iterator(UnorderedIterable& iterable, ID index);

	private:
		UnorderedIterable *m_iterable;
		ID m_index;

		friend class UnorderedIterable;
	};
	friend class iterator;

	iterator begin();
	iterator end();

	//////////////////////////////////////////////
	// Capacity

	bool empty() const;

	size_t size() const;

	void reserve(size_t size);

	size_t capacity() const;

	//////////////////////////////////////////////
	// Modification

	void clear();

	// Returns ID that can be used to retrieve the element.
	ID insert(const T& val);

	void remove(ID id);

private:
	struct Element
	{
		Element *next;
		T value;

		// NOTE: This will technically return true for a value at the end of the free list, 
		//       but the iterator should recognize being past the furthest element as being invalid.
		bool HasValue() { return (next == nullptr); }
	};

	// Hook up empty element list betwwen indeces. Last element points next to "end".
	// Returns first element.
	Element *InitElementList(ID begin, ID end, Element *endPointer = nullptr);

	Element *m_data;
	size_t m_size;
	size_t m_capacity;
	size_t m_furthestIndex;

	Element *m_freeListHead;
};

// Does the implementation pass tests?
bool UnorderedIterablePassesTests();
#include "UnorderedIterable.inl"

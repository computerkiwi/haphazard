/*
FILE: Stack.h
PRIMARY AUTHOR: Sweet

Declaration of Stack Type

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once
#include <cstdlib>


// Make into non-dynamic array?
template <typename T, std::size_t heap_size>
class Stack
{
	T m_heap[heap_size];
	void * m_data;
	std::size_t m_heapSize;

	std::size_t m_size;
public:
	explicit Stack(std::size_t heap_size);

	Stack(std::size_t object_size, std::size_t count);

	~Stack();

	std::size_t GetSize() const;

	T * push(T & object);

	T * push(T && object);

	void pop();
};




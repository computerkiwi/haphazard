/*
FILE: Stack.cpp
PRIMARY AUTHOR: Sweet

Function Bodies of Stack Type

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once
#include "Stack.h"


template <typename T, std::size_t heap_size>
Stack<T, heap_size>::Stack(std::size_t heap_size) : m_data(m_heap), m_heapSize(heap_size)
{
}


template <typename T, std::size_t heap_size>
Stack<T, heap_size>::Stack(std::size_t object_size, std::size_t count) : m_heap(malloc(object_size * count)), m_data(m_heap), m_heapSize(object_size * count)
{
}

template <typename T, std::size_t heap_size>
Stack<T, heap_size>::~Stack()
{
	free(m_heap);
}


template <typename T, std::size_t heap_size>
std::size_t Stack<T, heap_size>::GetSize() const
{
	return m_size;
}


template <typename T, std::size_t heap_size>
T * Stack<T, heap_size>::push(T & object)
{
	if (m_data < m_heap + m_heapSize)
	{
		*m_data = object;
		m_data += sizeof(T);
		return m_data - sizeof(T);
	}

	#if _DEBUG
		throw("Just add more to init size of stack.");
	#endif

	return nullptr;
}


template <typename T, std::size_t heap_size>
T * Stack<T, heap_size>::push(T && object)
{
	if (m_data < m_heap + m_heapSize)
	{
		*m_data = object;
		m_data += sizeof(T);
		return m_data - sizeof(T);
	}

	#if _DEBUG
		throw("Just add more to init size of stack.");
	#endif

	return nullptr;
}


template <typename T, std::size_t heap_size>
void Stack<T, heap_size>::pop()
{
	if (m_data > m_heap)
			m_data -= sizeof(T);
}


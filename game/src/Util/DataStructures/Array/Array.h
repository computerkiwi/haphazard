/*
FILE: Array.h
PRIMARY AUTHOR: Sweet

Contains the struct for Arrays

Copyright � 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include <cstddef>

template <typename T, std::size_t size>
struct Array
{
	std::size_t m_size = 0;
	T m_array[size];

	T * Push(T & object)
	{
		m_array[m_size++] = object;
		return &object;
	}

	T * Push(T && object)
	{
		m_array[m_size++] = object;
		return &object;
	}

	void Pop()
	{
		--m_size;
	}

	void Clear()
	{
		m_size = 0;
	}

	const T & operator[](std::size_t index) const
	{
		return m_array[index];
	}

	T & operator[](std::size_t index)
	{
		return m_array[index];
	}
};




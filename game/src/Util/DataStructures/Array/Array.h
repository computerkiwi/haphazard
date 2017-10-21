/*
FILE: Array.h
PRIMARY AUTHOR: Sweet

Contains the struct for Arrays

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include <cstddef>

template <typename T, std::size_t size>
struct Array
{
	std::size_t m_size = 0;
	T m_array[size];

	Array() : Array(T()) {}
	Array(T& init) 
	{
		for (size_t i = 0; i < size; i++)
		{
			m_array[i] = init;
		}
	}

	Array(T&& init)
	{
		for (size_t i = 0; i < size; i++)
		{
			m_array[i] = init;
		}
	}


	T * push_back(T & object)
	{
		m_array[m_size++] = object;
		return &object;
	}

	T * push_back(T && object)
	{
		m_array[m_size++] = object;
		return &object;
	}

	void pop()
	{
		--m_size;
	}

	void clear()
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




/*
FILE: MemberOffset.cpp
PRIMARY AUTHOR: Kiera Williams

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "meta.h"

namespace meta
{

	MemberOffset::MemberOffset(const char *name, Type *type, size_t offset) : m_name(name), m_type(type), m_offset(offset)
	{
	}

	//---------
	// Getters
	//---------
	std::string MemberOffset::GetName()
	{
		return m_name;
	}

	Type *MemberOffset::GetType()
	{
		return m_type;
	}

	const void *MemberOffset::Get(const void *object)
	{
		return reinterpret_cast<const char *>(object) + m_offset;
	}

	void *MemberOffset::Get(void *object)
	{
		return reinterpret_cast<char *>(object) + m_offset;
	}

	void MemberOffset::Set(void *object, const void *data)
	{
		void *dataPointer = Get(object);

		m_type->assignmentOperator(dataPointer, data);
	}
}
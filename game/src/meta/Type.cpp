/*
FILE: Type.cpp
PRIMARY AUTHOR: Kieran Williams

Copyright � 2017 DigiPen (USA) Corporation.
*/

#include "meta.h"
#include <assert.h>
#include <vector>

namespace meta
{

	Type::~Type()
	{
		delete m_pointerType;

		for (auto iter = m_members.begin(); iter != m_members.end(); ++iter)
		{
			delete iter->second;
		}
	}

	//---------
	// Getters
	//---------
	std::string Type::GetName()
	{
		return m_name;
	}

	size_t Type::GetSize()
	{
		return m_size;
	}

	Type *Type::GetPointerType()
	{
		// Construct the pointer type if we have to.
		if (m_pointerType == nullptr)
		{
			std::string pointerName = "ptr to ";
			pointerName += m_name;

			m_pointerType = new Type(sizeof(void *), pointerName.c_str(), internal::CopyConstructor<void *>, internal::MoveConstructor<void *>, internal::Assignment<void *>, internal::MoveAssignment<void *>, internal::Destructor<void *>, this);
		}

		return m_pointerType;
	}

	Type *Type::GetDereferenceType()
	{
		assert(IsPointerType());

		return m_dereferenceType;
	}

	Type *Type::GetDeepestDereference()
	{
		Type *type = this;

		while (type->m_dereferenceType != nullptr)
		{
			type = type->m_dereferenceType;
		}

		return type;
	}

	bool Type::IsPointerType()
	{
		return (m_dereferenceType != nullptr);
	}

	void Type::RegisterMember(const char *name, Type *type, size_t offset)
	{
		assert(m_members.find(name) == m_members.end());

		m_members.emplace(name, new MemberOffset(name, type, offset));
	}

	std::vector<Member *> Type::GetMembers()
	{
		std::vector<Member *> membersVector;

		for (auto iter = m_members.begin(); iter != m_members.end(); ++iter)
		{
			membersVector.push_back(iter->second);
		}

		return membersVector;
	}

	// Returns nullptr if the member name doesn't exist.
	Member *Type::GetMember(const char *name)
	{
		auto memberIter = m_members.find(name);

		if (memberIter == m_members.end())
		{
			return nullptr;
		}
		else
		{
			return memberIter->second;
		}
	}
}


/*
FILE: MemberGetSet.inl
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

namespace meta
{
	//---------
	// Getters
	//---------
	template <typename BaseType, typename MemberType>
	std::string MemberGetSet<BaseType, MemberType>::GetName()
	{
		return m_name;
	}

	template <typename BaseType, typename MemberType>
	Type *MemberGetSet<BaseType, MemberType>::GetType()
	{
		return GetTypePointer<MemberType>();
	}
	
	template <typename BaseType, typename MemberType>
	const void *MemberGetSet<BaseType, MemberType>::Get(const void *object)
	{
		// It's 1:30 AM and I can't be bothered to deal with this const where it shouldn't really matter. Fuck it.
		BaseType *baseObj = reinterpret_cast<BaseType *>(const_cast<void *>(object));

		if (m_getter != nullptr)
		{
			m_getterBuffer = (baseObj->*m_getter)();
			return &m_getterBuffer;
		}
		else if (m_refGetter != nullptr)
		{
			return &(baseObj->*m_refGetter)();
		}

		assert(false); // One of the above conditions should be true.
		return nullptr;
	}

	template <typename BaseType, typename MemberType>
	void MemberGetSet<BaseType, MemberType>::Set(void *object, const void *data)
	{
		BaseType *baseObj = reinterpret_cast<BaseType *>(object);
		const MemberType *dataPtr = reinterpret_cast<const MemberType *>(data);

		if (m_setter != nullptr)
		{
			(baseObj->*m_setter)(*dataPtr);
			return;
		}
		else if (m_refSetter != nullptr)
		{
			(baseObj->*m_refSetter)(*dataPtr);
			return;
		}
		else if (m_refGetter != nullptr)
		{
			(baseObj->*m_refGetter)() = *reinterpret_cast<const MemberType *>(data);
			return;
		}

		assert(false); // One of the above conditions should be true.
	}
}

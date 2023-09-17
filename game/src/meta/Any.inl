/*
FILE: Any.inl
PRIMARY AUTHOR: Kiera Williams

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

namespace meta
{

	template <typename T>
	Any::Any(const T& object) : m_type(GetTypePointer<T>())
	{
		if (sizeof(T) > MAX_SIZE)
		{
			m_dataPointer = new char[sizeof(T)];
			new (m_dataPointer) T(object);
			m_usesPointer = true;
		}
		else
		{
			// Copy the data into our data storage.
			new (m_data) T(object);
			m_usesPointer = false;
		}
	}

	template <typename T>
	T& Any::GetData()
	{
		Assert(GetTypePointer<T>() == m_type);

		if (m_usesPointer)
		{
			return *reinterpret_cast<T *>(m_dataPointer);
		}
		else
		{
			return *reinterpret_cast<T *>(&m_data);
		}
	}

	template <typename T>
	void Any::SetData(const T& data)
	{
		Assert(GetTypePointer<T>() == m_type);

		if (m_usesPointer)
		{
			*reinterpret_cast<T *>(m_dataPointer) = data;
		}
		else
		{
			*reinterpret_cast<T *>(&m_data) = data;
		}
	}

	//-----------------------
	// Member Getter/Setters
	//-----------------------

	template <typename T>
	void Any::SetMember(Member *member, const T& value)
	{
		Assert(!m_type->IsPointerType());
		Assert(member->GetType() == GetTypePointer<T>());

		member->Set(GetDataPointer(), &value);
	}

	template <typename T>
	void Any::SetMember(const char *memberName, const T& value)
	{
		Assert(!m_type->IsPointerType());
		Member *member = m_type->GetMember(memberName);

		Assert(member->GetType() == GetTypePointer<T>());

		member->Set(GetDataPointer(), &value);
	}

	template <typename T>
	T Any::GetMember(Member *member) const
	{
		Assert(!m_type->IsPointerType());
		Assert(member->GetType() == GetTypePointer<T>());

		return *reinterpret_cast<T *>(member->Get(GetDataPointer()));
	}

	template <typename T>
	T Any::GetMember(const char *memberName) const
	{
		Assert(!m_type->IsPointerType());
		Member *member = m_type->GetMember(memberName);

		Assert(member->GetType() == GetTypePointer<T>());

		return *reinterpret_cast<const T *>(member->Get(GetDataPointer()));
	}

	//-------------------------------
	// Pointer Member Getter/Setters
	//-------------------------------

	template <typename T>
	void Any::SetPointerMember(Member *member, const T& value)
	{
		Assert(m_type->IsPointerType());
		Assert(member->GetType() == GetTypePointer<T>());

		member->Set(GetDeepestDataPointer(), &value);
	}

	template <typename T>
	void Any::SetPointerMember(const char *memberName, const T& value)
	{
		Assert(m_type->IsPointerType());
		Member *member = m_type->GetDeepestDereference()->GetMember(memberName);

		Assert(member->GetType() == GetTypePointer<T>());

		member->Set(GetDeepestDataPointer(), &value);
	}

	template <typename T>
	T Any::GetPointerMember(Member *member) const
	{
		Assert(m_type->IsPointerType());
		Assert(member->GetType() == GetTypePointer<T>());

		return *reinterpret_cast<T *>(member->Get(GetDeepestDataPointer()));
	}

	template <typename T>
	T Any::GetPointerMember(const char *memberName) const
	{
		Assert(m_type->IsPointerType());
		Member *member = m_type->GetDeepestDereference()->GetMember(memberName);

		Assert(member->GetType() == GetTypePointer<T>());

		return *reinterpret_cast<T *>(member->Get(GetDeepestDataPointer()));
	}
}

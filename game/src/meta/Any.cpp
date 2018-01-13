/*
FILE: Any.cpp
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2017 DigiPen (USA) Corporation.
*/

#include "meta.h"
#include "rapidjson/document.h"
#include "GameObjectSystem/GameObject.h"

namespace meta
{
	Any::Any(const void *object, Type *type) : m_type(type)
	{
		if (type->GetSize() > MAX_SIZE)
		{
			m_dataPointer = new char[type->GetSize()];
			type->copyConstructor(m_dataPointer, object);
			m_usesPointer = true;
		}
		else
		{
			type->copyConstructor(m_data, object);
			m_usesPointer = false;
		}
	}

	Any::Any(const Any& other) : m_type(other.m_type), m_usesPointer(other.m_usesPointer)
	{
		if (m_usesPointer)
		{
			m_dataPointer = new char[m_type->GetSize()];
			m_type->copyConstructor(m_dataPointer, other.m_dataPointer);
		}
		else
		{
			m_type->copyConstructor(m_data, other.m_data);
		}
	}

	Any::Any(Any&& other) : m_type(other.m_type), m_usesPointer(other.m_usesPointer)
	{
		if (m_usesPointer)
		{
			m_dataPointer = other.m_dataPointer;
			other.m_dataPointer = nullptr;
		}
		else
		{
			m_type->moveConstructor(m_data, other.m_data);
		}
	}

	// Deserialize constructor
	Any::Any(rapidjson::Value& jsonValue)
	{
		Assert(jsonValue.IsObject());
		m_type = GetTypeByName(jsonValue["meta_type_name"].GetString());

		if (m_type->GetSize() > MAX_SIZE)
		{
			m_dataPointer = new char[m_type->GetSize()];
			m_type->DeserializeConstruct(m_dataPointer, jsonValue);
			m_usesPointer = true;
		}
		else
		{
			m_type->DeserializeConstruct(m_data, jsonValue);
			m_usesPointer = false;
		}
	}

	Any& Any::operator=(const Any& other)
	{
		// If we're dealing with different types just construct a new copy and put it here.
		if (m_type != other.m_type)
		{
			this->~Any();
			new (this) Any(other);
			return *this;
		}

		if (m_usesPointer)
		{
			m_type->assignmentOperator(m_dataPointer, other.m_dataPointer);
		}
		else
		{
			m_type->assignmentOperator(m_data, other.m_data);
		}

		return *this;
	}

	Any& Any::operator=(Any&& other)
	{
		// If we're dealing with different types just construct a new copy and put it here.
		if (m_type != other.m_type)
		{
			this->~Any();
			new (this) Any(other);
			return *this;
		}

		if (m_usesPointer)
		{
			m_type->moveAssignmentOperator(m_dataPointer, other.m_dataPointer);
		}
		else
		{
			m_type->moveAssignmentOperator(m_data, other.m_data);
		}

		return *this;
	}

	Any::~Any()
	{
		if (m_usesPointer)
		{
			// Could be nullptr due to being moved out of.
			if (m_dataPointer != nullptr)
			{
				m_type->destructor(m_dataPointer);
				delete[] m_dataPointer;
			}
		}
		else
		{
			m_type->destructor(m_data);
		}
	}

	// Dereference operator.
	Any Any::operator*()
	{
		Assert(m_type->IsPointerType());

		// Void pointer to the data we're pointing at.
		void *dataPointer;
		if (m_usesPointer)
		{
			dataPointer = *reinterpret_cast<void **>(m_dataPointer);
		}
		else
		{
			dataPointer = &m_data;
		}

		return Any(dataPointer, m_type->GetDereferenceType());
	}

	bool Any::IsNullPtr()
	{
		// We can only be nullptr if we represent a pointer in the first place.
		if (!m_type->IsPointerType())
		{
			return false;
		}

		// Get the actual pointer this Any represents.
		void *pointer = *reinterpret_cast<void **>(GetDataPointer());

		return (pointer == nullptr);
	}

	//-----------------------
	// Member Getter/Setters
	//-----------------------

	void Any::SetMember(Member *member, const Any& value)
	{
		Assert(!m_type->IsPointerType());
		Assert(member->GetType() == value.GetType());

		member->Set(GetDataPointer(), value.GetDataPointer());
	}

	void Any::SetMember(const char *memberName, const Any& value)
	{
		Assert(!m_type->IsPointerType());
		Member *member = m_type->GetMember(memberName);

		Assert(member->GetType() == value.GetType());

		member->Set(GetDataPointer(), value.GetDataPointer());
	}

	Any Any::GetMember(Member *member)
	{
		Assert(!m_type->IsPointerType());
		return Any(member->Get(GetDataPointer()), member->GetType());
	}

	Any Any::GetMember(const char *memberName)
	{
		Assert(!m_type->IsPointerType());
		Member *member = m_type->GetMember(memberName);

		Assert(member != nullptr);

		return Any(member->Get(GetDataPointer()), member->GetType());
	}

	//-------------------------------
	// Pointer Member Getter/Setters
	//-------------------------------

	void Any::SetPointerMember(Member *member, const Any& value)
	{
		Assert(m_type->IsPointerType());
		Assert(member->GetType() == value.GetType());

		member->Set(GetDeepestDataPointer(), value.GetDataPointer());
	}

	void Any::SetPointerMember(const char *memberName, const Any& value)
	{
		Assert(m_type->IsPointerType());
		Member *member = m_type->GetDeepestDereference()->GetMember(memberName);

		Assert(member->GetType() == value.GetType());

		member->Set(GetDeepestDataPointer(), value.GetDataPointer());
	}

	Any Any::GetPointerMember(Member *member)
	{
		Assert(m_type->IsPointerType());
		return Any(member->Get(GetDeepestDataPointer()), member->GetType());
	}

	Any Any::GetPointerMember(const char *memberName)
	{
		Assert(m_type->IsPointerType());
		Member *member = m_type->GetDeepestDereference()->GetMember(memberName);

		Assert(member != nullptr);

		return Any(member->Get(GetDeepestDataPointer()), member->GetType());
	}

	//-----------------
	// General Getters
	//-----------------
	Type *Any::GetType() const
	{
		return m_type;
	}

	//---------------
	// Serialization
	//---------------
	rapidjson::Value Any::Serialize(rapidjson::Document::AllocatorType& allocator)
	{
		return m_type->GetDeepestDereference()->Serialize(GetDeepestDataPointer(), allocator);
	}

	void Any::SetGameObjectID(GameObject_ID id)
	{
		m_type->GetDeepestDereference()->SetGameObjectID(GetDeepestDataPointer(), id);
	}

	void *Any::GetDataPointer()
	{
		if (m_usesPointer)
		{
			return m_dataPointer;
		}
		else
		{
			return m_data;
		}
	}

	const void *Any::GetDataPointer() const
	{
		if (m_usesPointer)
		{
			return m_dataPointer;
		}
		else
		{
			return m_data;
		}
	}

	// Points to the actual data even if we have a pointer type.
	void *Any::GetDeepestDataPointer()
	{
		// Start at the top level.
		void *data = GetDataPointer();

		Type *type = m_type;
		// Chase the pointer down the types.
		while (type->IsPointerType())
		{
			type = type->GetDereferenceType();
			data = *reinterpret_cast<void **>(data);
		}

		return data;
	}

	const void *Any::GetDeepestDataPointer() const
	{
		// Makes the casting clearer.
		typedef const void * ConstVoidPtr;

		// Start at the top level.
		ConstVoidPtr data = GetDataPointer();

		Type *type = m_type;
		// Chase the pointer down the types.
		while (type->IsPointerType())
		{
			type = type->GetDereferenceType();
			data = *reinterpret_cast<const ConstVoidPtr *>(data);
		}

		return data;
	}
}

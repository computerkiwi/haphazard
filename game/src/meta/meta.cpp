/*******************************************************
FILE: meta.cpp

PRIMARY AUTHOR: Kieran Williams

PROJECT: <project_name>

Copyright 2017 DigiPen (USA) Corporation.
*******************************************************/

#include "meta.h"

namespace meta
{

	unsigned int meta::internal::TypeIdentifier::CountId()
	{
		return _idCounter++;
	}

	unsigned int meta::internal::TypeIdentifier::_idCounter = 0;

	std::map<meta::internal::TypeID, meta::Type> meta::internal::typeMap;

	// Register some default types.
	META_REGISTER(int);
	META_REGISTER(char);
	META_REGISTER(float);
	META_REGISTER(double);

	meta::AnyPointer meta::Type::GetProperty(AnyPointer& obj, const char *propName) const
	{
		// Walk through the properties until we find one with the proper name.
		for (MemberProperty *property : _properties)
		{
			if (strcmp(property->Name(), propName) == 0)
			{
				return property->Get(obj);
			}
		}

		// If we received the name of an invalid property, the caller is at fault. Just crash it.
		assert(false && "Tried to get an invalid property.");
		return obj;
	}

	void meta::Type::SetProperty(AnyPointer& obj, const char *propName, AnyPointer& value)
	{
		// Walk through the properties until we find one with the proper name.
		for (MemberProperty *property : _properties)
		{
			if (strcmp(property->Name(), propName) == 0)
			{
				property->Set(obj, value);
				return;
			}
		}

		// Do nothing if the name was invalid.
		return;
	}

	AnyPointer AnyPointer::GetProperty(const char *propertyName)
	{
		return _typeInfo->GetProperty(*this, propertyName);
	}

	void AnyPointer::SetProperty(const char *propertyName, AnyPointer& value)
	{
		_typeInfo->SetProperty(*this, propertyName, value);
	}
}

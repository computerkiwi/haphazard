/*******************************************************
FILE: meta.cpp

PRIMARY AUTHOR: Kieran Williams

PROJECT: <project_name>

Copyright 2017 DigiPen (USA) Corporation.
*******************************************************/

#include "meta.h"
#include <cstring>

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
	META_REGISTER(bool);
	META_REGISTER(long);

	PropertySignature *Type::GetProperty(const char *propName)
	{
		// Walk through the properties until we find one with the proper name.
		for (PropertySignature *property : _properties)
		{
			if (strcmp(property->Name(), propName) == 0)
			{
				return property;
			}
		}

		// If we received the name of an invalid property, the caller is at fault. Just crash it.
		assert(false && "Tried to get an invalid property.");
		return nullptr;
	}

	bool Type::HasProperty(const char *propertyName)
	{
		// Run through the properties until we find one with the desired name.
		for (PropertySignature *prop : _properties)
		{
			if (strcmp(prop->Name(), propertyName) == 0)
			{
				return true;
			}
		}

		// If we didn't find anything, the type doesn't have the property.
		return false;
	}

	// Returns a vector of information on a type's properties.
	std::vector<PropertySignature *> Type::GetPropertiesInfo()
	{
		return _properties;
	}

	Any Any::GetProperty(const char *propertyName)
	{
		return GetProperty(*_typeInfo->GetProperty(propertyName));
	}

	Any Any::GetProperty(PropertySignature& property)
	{
		return property.ExtractValue(m_data);
	}

	void Any::SetProperty(const char *propertyName, Any& value)
	{
		SetProperty(*_typeInfo->GetProperty(propertyName), value);
	}

	void Any::SetProperty(PropertySignature& property, Any& value)
	{
		if (m_isPointer)
		{
			property.SetValue(m_pointer, value);
		}
		else
		{
			property.SetValue(m_data, value);
		}
	}

	Type *Any::GetType()
	{
		return _typeInfo;
	}
}

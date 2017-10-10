/*
FILE: BuiltInRegistration.h
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "meta.h"

#define SERIALIZE_INTERGAL_TYPE_FUNCTION(TYPE)\
template <>\
rapidjson::Value SerializeBasicType<TYPE>(const void *object, rapidjson::Document::AllocatorType& allocator)\
{\
	(void)allocator;\
\
	rapidjson::Value val;\
	val.SetInt(static_cast<int>(*static_cast<const TYPE *>(object)));\
\
	return val;\
}

// Serialize functions.
namespace
{
	template <typename T>
	rapidjson::Value SerializeBasicType(const void *object, rapidjson::Document::AllocatorType& allocator)
	{
		// Unused parameter
		(void)allocator;

		rapidjson::Value val;
		val.Set<T>(*static_cast<const T *>(object));

		return val;
	}
	
	SERIALIZE_INTERGAL_TYPE_FUNCTION(char)
	SERIALIZE_INTERGAL_TYPE_FUNCTION(unsigned char)
	SERIALIZE_INTERGAL_TYPE_FUNCTION(signed char)
	SERIALIZE_INTERGAL_TYPE_FUNCTION(short)
	SERIALIZE_INTERGAL_TYPE_FUNCTION(unsigned short)
	SERIALIZE_INTERGAL_TYPE_FUNCTION(long)
	SERIALIZE_INTERGAL_TYPE_FUNCTION(unsigned long)
	SERIALIZE_INTERGAL_TYPE_FUNCTION(wchar_t)

	template <>
	rapidjson::Value SerializeBasicType<long double>(const void *object, rapidjson::Document::AllocatorType& allocator)
	{
		(void)allocator;
	
		rapidjson::Value val;
		val.SetDouble(static_cast<double>(*static_cast<const long double *>(object)));
	
		return val;
	}

}

#define DefineBasicType(TYPE) META_DefineType(TYPE); META_DefineSerializeFunction(TYPE, SerializeBasicType<TYPE>)

#define DefIntType(TYPE) DefineBasicType(TYPE); DefineBasicType(unsigned TYPE); DefineBasicType(signed TYPE)\

namespace meta
{
	class BuiltInRegistration
	{
		META_NAMESPACE(::meta);
		META_REGISTER(BuiltInRegistration)
		{
			// List taken from https://www.tutorialspoint.com/cplusplus/cpp_data_types.htm

			DefIntType(char);
			DefIntType(int);
			DefIntType(short);
			DefIntType(long);
			DefIntType(long long);

			DefineBasicType(float);
			DefineBasicType(double);
			DefineBasicType(long double);
			DefineBasicType(wchar_t);
		}
	};
}
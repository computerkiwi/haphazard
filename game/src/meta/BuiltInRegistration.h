/*
FILE: BuiltInRegistration.h
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include <string>

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

#define DESERIALIZE_INTERGAL_TYPE_FUNCTION(TYPE)\
template <>\
void DeserializeAssignBasicType<TYPE>(void * objectPtr, rapidjson::Value& jsonObject)\
{\
	TYPE& object = *reinterpret_cast<TYPE *>(objectPtr);\
\
	object = static_cast<TYPE>(jsonObject.GetInt64());\
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

	template <typename T>
	void DeserializeAssignBasicType(void *objectPtr, rapidjson::Value& jsonObject)
	{
		T& object = *reinterpret_cast<T *>(objectPtr);

		object = jsonObject.Get<T>();
	}

	DESERIALIZE_INTERGAL_TYPE_FUNCTION(char)
	DESERIALIZE_INTERGAL_TYPE_FUNCTION(unsigned char)
	DESERIALIZE_INTERGAL_TYPE_FUNCTION(signed char)
	DESERIALIZE_INTERGAL_TYPE_FUNCTION(short)
	DESERIALIZE_INTERGAL_TYPE_FUNCTION(unsigned short)
	DESERIALIZE_INTERGAL_TYPE_FUNCTION(long)
	DESERIALIZE_INTERGAL_TYPE_FUNCTION(unsigned long)
	DESERIALIZE_INTERGAL_TYPE_FUNCTION(wchar_t)

	template <>
	void DeserializeAssignBasicType<long double>(void *objectPtr, rapidjson::Value& jsonObject)
	{
		long double& object = *reinterpret_cast<long double *>(objectPtr);

		object = static_cast<long double>(jsonObject.GetDouble());
	}

	// std::string
	rapidjson::Value SerializeString(const void *object, rapidjson::Document::AllocatorType& allocator)
	{
		const std::string& string = *reinterpret_cast<const std::string *>(object);

		rapidjson::Value jsonString;
		jsonString.SetString(string.c_str(), allocator);

		return jsonString;
	}
	void DeserializeAssignString(void *objectPtr, rapidjson::Value& jsonObject)
	{
		std::string& string = *reinterpret_cast<std::string *>(objectPtr);

		assert(jsonObject.IsString());
		string = jsonObject.GetString();
	}

	rapidjson::Value SerializeBool(const void *object, rapidjson::Document::AllocatorType& allocator)
	{
		const bool& boolData = *reinterpret_cast<const bool *>(object);

		rapidjson::Value jsonBool;
		jsonBool.SetBool(boolData);

		return jsonBool;
	}
	void DeserializeAssignBool(void *objectPtr, rapidjson::Value& jsonObject)
	{
		bool& boolData = *reinterpret_cast<bool *>(objectPtr);

		assert(jsonObject.IsBool());
		boolData = jsonObject.GetBool();
	}
}

#define DefineBasicType(TYPE) META_DefineType(TYPE); META_DefineSerializeFunction(TYPE, SerializeBasicType<TYPE>); META_DefineDeserializeAssignFunction(TYPE, DeserializeAssignBasicType<TYPE>)

#define DefIntType(TYPE) DefineBasicType(TYPE); DefineBasicType(unsigned TYPE); DefineBasicType(signed TYPE)\

namespace meta
{
	class BuiltInRegistration
	{
		META_NAMESPACE(::meta);
		META_PREREGISTER(BuiltInRegistration)
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

			META_DefineType(bool);
			META_DefineSerializeFunction(bool, SerializeBool);
			META_DefineDeserializeAssignFunction(bool, DeserializeAssignBool);

			META_DefineType(std::string);
			META_DefineSerializeFunction(std::string, SerializeString);
			META_DefineDeserializeAssignFunction(std::string, DeserializeAssignString);
		}
	};
}
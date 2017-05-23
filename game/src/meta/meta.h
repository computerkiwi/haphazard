/*******************************************************
FILE: meta.h

PRIMARY AUTHOR: Kieran Williams

PROJECT: <project_name>

Copyright 2017 DigiPen (USA) Corporation.
*******************************************************/

#pragma once

#include <map>
#include <assert.h>

//
// INTERFACE
//
namespace meta
{
	// Forward declaration for Type class.
	class Type;

	// Represents a pointer to a variable of a certain type.
	class Pointer
	{
	public:

	private:
		void *_pointer;
		const Type *_typeInfo;
	};

	// Represents a type or struct.
	class Type
	{
	public:
		Type(std::string name) : _name(name) {}
		const std::string& Name() const { return _name; };

	private:
		std::string _name;
	};

}

//
// IMPLEMENTATION
//
namespace meta
{
	namespace internal
	{
		//
		// Type Identifier 
		//

		typedef unsigned int TypeID;

		// Identifier
		class TypeIdentifier
		{
		public:
			template <typename T> static TypeID Get();
		private:
			static TypeID CountId();
			static TypeID _idCounter;
			TypeID _id;
		};

		template <typename T>
		unsigned int TypeIdentifier::Get()
		{
			static bool hasRun = false;
			static TypeID identifier = 0;

			// Generate a new id if we haven't gotten one for this type before.
			if (!hasRun)
			{
				identifier = CountId();
				hasRun = true;
			}

			return identifier;
		}

		//
		// 
		//

		extern std::map<TypeID, Type> typeMap;

		//
		// Registration
		//

		template <typename T>
		Type *RegisterType(std::string name)
		{
			TypeID id = TypeIdentifier::Get<T>();

			assert(typeMap.find(id) == typeMap.end() && "Registering an already registered type.");

			// Add a new type object and store the pointer to it.
			Type *typePointer = &(typeMap.insert(std::pair<TypeID, Type>(id, Type(name))).first->second);

			return typePointer;
		}

		#define META_REGISTER(TYPENAME) const ::meta::Type *tempPointer_##TYPENAME = ::meta::internal::RegisterType<TYPENAME>(#TYPENAME)

		//
		// Get Registered Type
		//

		template <typename T>
		const Type *GetType()
		{
			TypeID id = TypeIdentifier::Get<T>();
			auto typeIt = typeMap.find(id);

			// Return a null pointer if we couldn't find the type.
			if (typeIt == typeMap.end())
			{
				return nullptr;
			}

			return &(typeIt->second);
		}

		// Convenience wrapper so we can just pass in an object.
		template <typename T>
		const Type *GetType(T obj)
		{
			return GetType<T>();
		}
	}
}

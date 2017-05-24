/*******************************************************
FILE: meta.h

PRIMARY AUTHOR: Kieran Williams

PROJECT: <project_name>

Copyright 2017 DigiPen (USA) Corporation.
*******************************************************/

#pragma once

#include <map>
#include <vector>
#include <assert.h>

//
// INTERFACE
//
namespace meta
{
	// Forward declaration for Type class.
	class Type;

	// Represents a pointer to a variable of a certain type.
	class AnyPointer
	{
	public:
		template <typename T> AnyPointer(T *pointer) : _pointer(pointer), _typeInfo(internal::GetType<T>()) {}
		
		template <typename T> T *GetPointer() { assert(_typeInfo == internal::GetType<T>()); return reinterpret_cast<T *>(_pointer); }

		AnyPointer GetProperty(const char *propertyName);
		void SetProperty(const char *propertyName, AnyPointer& value);

	private:
		void *_pointer;
		Type *_typeInfo;
	};

	class MemberProperty
	{
	public:
		MemberProperty(const char *name) : _name(name) {}

		virtual AnyPointer Get(AnyPointer& obj) = 0;
		virtual void Set(AnyPointer& obj, AnyPointer& value) = 0;
		const char *Name() { return _name; }

	private:
		const char *_name;
	};

	template <typename BaseType, typename MemberType>
	class TemplatedMember : public MemberProperty
	{
	public:
		TemplatedMember(const char *name, MemberType BaseType::*member) : MemberProperty(name), _member(member) {}

		virtual AnyPointer Get(AnyPointer& obj)
		{
			BaseType *pObj = obj.GetPointer<BaseType>();
			return AnyPointer(&(pObj->*_member));
		}

		virtual void Set(AnyPointer& obj, AnyPointer& value)
		{
			BaseType *pObj = obj.GetPointer<BaseType>();
			(pObj->*_member) = *value.GetPointer<MemberType>();
		}

	private:
		MemberType BaseType::*_member;
	};

	// Represents a type or struct.
	class Type
	{
	public:
		Type(std::string name) : _name(name) {}

		const std::string& Name() const { return _name; };

		template <typename BaseType, typename MemberType> Type *RegisterProperty(const char *name, MemberType BaseType::*member) { _properties.push_back(new TemplatedMember<BaseType, MemberType>(name, member));  return this; }

		AnyPointer GetProperty(AnyPointer& obj, const char *propName) const;
		void SetProperty(AnyPointer& obj, const char *propName, AnyPointer& value);

	private:
		std::string _name;
		std::vector<MemberProperty *> _properties;
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
		Type *GetType()
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
		Type *GetType(T obj)
		{
			return GetType<T>();
		}
	}
}

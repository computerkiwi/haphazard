/*******************************************************
FILE: meta.h

PRIMARY AUTHOR: Kieran Williams

PROJECT: <project_name>

Copyright 2017 DigiPen (USA) Corporation.
*******************************************************/

#pragma once

#include <string>
#include <map>
#include <vector>
#include <assert.h>

//
// INTERFACE
//
namespace meta
{
	// Forward declarations for Type class.
	class Type;
	class PropertySignature;

	// Represents a pointer to a variable of a certain type.
	#define ANY_MAXSIZE 64
	class Any
	{
	public:
		template <typename T> Any(T  value);
		template <typename T> Any(T *value);

		template <typename T> T Get();
		template <typename T> T *GetPointer();

		Any GetProperty(const char *propertyName);
		Any GetProperty(PropertySignature& property);

		void SetProperty(const char *propertyName, Any& value);
		void SetProperty(PropertySignature& property, Any& value);
		Type *GetType();

	private:
		union
		{
			char m_data[ANY_MAXSIZE];
			void *m_pointer;
		};
		Type *_typeInfo;
		bool m_isPointer;
	};

	class PropertySignature
	{
	public:
		PropertySignature(const char *name) : _name(name) {}

		virtual Type *GetType() = 0;
		virtual Any ExtractValue(void *baseObject) = 0;
		virtual void SetValue(void *baseObject, Any& value) = 0;
		const char *Name() { return _name; }

	private:
		const char *_name;
	};

	template <typename BaseType, typename MemberType>
	class TemplatedMember : public PropertySignature
	{
	public:
		TemplatedMember(const char *name, MemberType BaseType::*member) : PropertySignature(name), _member(member) {}
	
		virtual Type *GetType();

		virtual Any ExtractValue(void *baseObject);

		virtual void SetValue(void *baseObject, Any& value);

	private:
		MemberType BaseType::*_member;
	};

	template <typename BaseType, typename MemberType>
	class FunctionMember : public PropertySignature
	{
	public:
		FunctionMember(const char *name, MemberType (BaseType::*getter)(), void (BaseType::*setter)(const MemberType&)) : PropertySignature(name), m_getter(getter), m_setter(setter) {}

		virtual Type *GetType();

	private:
		MemberType (BaseType::*m_getter)();
		void (BaseType::*m_setter)(const MemberType &);
	};

	// Represents a type or struct.
	class Type
	{
	public:
		Type(std::string name) : _name(name) {}

		const std::string& Name() const { return _name; };

		template <typename BaseType, typename MemberType> Type& RegisterProperty(const char *name, MemberType BaseType::*member) { _properties.push_back(new TemplatedMember<BaseType, MemberType>(name, member));  return *this; }
		template <typename BaseType, typename MemberType> Type& RegisterProperty(const char *name, MemberType (BaseType::*getter)(), void (BaseType::*setter)(const MemberType&))
		{
			//TODO: Make this work at all.
			//_properties.push_back(new FunctionMember<BaseType, MemberType>( name, getter, setter));
			return *this;
		}

		PropertySignature *GetProperty(const char *propName);
		std::vector<PropertySignature *> GetPropertiesInfo();
		bool HasProperty(const char *propertyName);

	private:
		std::string _name;
		std::vector<PropertySignature *> _properties;
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
		Type& RegisterType(std::string name)
		{
			TypeID id = TypeIdentifier::Get<T>();

			assert(typeMap.find(id) == typeMap.end() && "Registering an already registered type.");

			// Add a new type object and return the reference.
			return typeMap.insert(std::pair<TypeID, Type>(id, Type(name))).first->second;
		}

		#define META_REGISTER(TYPENAME) const ::meta::Type& tempPointer_##TYPENAME = ::meta::internal::RegisterType<TYPENAME>(#TYPENAME)

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

	//
	// Any Functions.
	//

	template <typename T> Any::Any(T value) : _typeInfo(internal::GetType<T>()), m_isPointer(false)
	{
		// Make sure we're not getting a value too big.
		assert(sizeof(value) <= sizeof(m_data));

		// Stuff the value into the data storage.
		T *dataPointer = reinterpret_cast<T *>(&m_data);
		*dataPointer = value;
	}

	template <typename T> Any::Any(T *value) : _typeInfo(internal::GetType<T>()), m_isPointer(true)
	{
		// Make sure we're not getting a value too big.
		assert(sizeof(*value) <= sizeof(m_data));

		// Store the pointer to the value.
		m_pointer = value;
	}

	template <typename T> T Any::Get()
	{
		return *GetPointer<T>();
	}

	template <typename T> T *Any::GetPointer()
	{
		// Make sure we're getting the right data type.
		assert(_typeInfo == internal::GetType<T>());

		if (m_isPointer)
		{
			return reinterpret_cast<T *>(m_pointer);
		}
		else
		{
			T *valuePointer = reinterpret_cast<T *>(&m_data);
			return valuePointer;
		}
	}


	//
	// TemplatedMember Functions.
	//

	template <typename BaseType, typename MemberType>
	Type *TemplatedMember<BaseType, MemberType>::GetType()
	{
		return internal::GetType<MemberType>();
	}

	template <typename BaseType, typename MemberType>
	Any TemplatedMember<BaseType, MemberType>::ExtractValue(void *baseObjectPointer)
	{
		// Unsafe if the passed void pointer isn't of type BaseType.

		BaseType *baseObject = reinterpret_cast<BaseType *>(baseObjectPointer);

		return Any(baseObject->*_member);
	}

	template <typename BaseType, typename MemberType>
	void TemplatedMember<BaseType, MemberType>::SetValue(void *baseObjectPointer, Any& value)
	{
		// Unsafe if the passed void pointer isn't of type BaseType.
		// Make sure the value is of type MemberType.
		assert(value.GetType() == internal::GetType<MemberType>());

		BaseType *baseObject = reinterpret_cast<BaseType *>(baseObjectPointer);

		baseObject->*_member = value.Get<MemberType>();
	}

	//
	// FunctionMember Functions
	//

	template <typename BaseType, typename MemberType>
	Type *FunctionMember<BaseType, MemberType>::GetType()
	{
		return internal::GetType<MemberType>();
	}
}

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
	// Forward declaration for Type class.
	class Type;

	// Represents a pointer to a variable of a certain type.
	class AnyPointer
	{
	public:
		template <typename T> AnyPointer(T *pointer);
		
		template <typename T> T *GetPointer();

		AnyPointer GetProperty(const char *propertyName);
		void SetProperty(const char *propertyName, AnyPointer& value);
		Type *GetType();

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
		virtual Type *GetType() = 0;
		const char *Name() { return _name; }

	private:
		const char *_name;
	};

	template <typename BaseType, typename MemberType>
	class TemplatedMember : public MemberProperty
	{
	public:
		TemplatedMember(const char *name, MemberType BaseType::*member) : MemberProperty(name), _member(member) {}

		virtual AnyPointer Get(AnyPointer& obj);
	
		virtual void Set(AnyPointer& obj, AnyPointer& value);
	
		virtual Type *GetType();

	private:
		MemberType BaseType::*_member;
	};

	template <typename BaseType, typename MemberType>
	class FunctionMember : public MemberProperty
	{
	public:
		FunctionMember(const char *name, MemberType (BaseType::*getter)(), void (BaseType::*setter)(const MemberType&)) : MemberProperty(name), m_getter(getter), m_setter(setter) {}

		virtual AnyPointer Get(AnyPointer& obj);

		virtual void Set(AnyPointer& obj, AnyPointer& value);

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
			_properties.push_back(new FunctionMember<BaseType, MemberType>( name, getter, setter));
			return *this;
		}

		AnyPointer GetProperty(AnyPointer& obj, const char *propName) const;
		void SetProperty(AnyPointer& obj, const char *propName, AnyPointer& value);
		std::vector<MemberProperty *> GetPropertiesInfo();
		bool HasProperty(const char *propertyName);

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

	template <typename T> AnyPointer::AnyPointer(T *pointer) : _pointer(pointer), _typeInfo(internal::GetType<T>())
	{
	}

	template <typename T> T *AnyPointer::GetPointer() 
	{ 
		assert(_typeInfo == internal::GetType<T>()); 
		return reinterpret_cast<T *>(_pointer); 
	}


	//
	// TemplatedMember Functions.
	//

	template <typename BaseType, typename MemberType>
	AnyPointer TemplatedMember<BaseType, MemberType>::Get(AnyPointer& obj)
	{
		BaseType *pObj = obj.GetPointer<BaseType>();
		return AnyPointer(&(pObj->*_member));
	}

	template <typename BaseType, typename MemberType>
	void TemplatedMember<BaseType, MemberType>::Set(AnyPointer& obj, AnyPointer& value)
	{
		BaseType *pObj = obj.GetPointer<BaseType>();
		(pObj->*_member) = *value.GetPointer<MemberType>();
	}

	template <typename BaseType, typename MemberType>
	Type *TemplatedMember<BaseType, MemberType>::GetType()
	{
		return internal::GetType<MemberType>();
	}

	//
	// FunctionMember Functions
	//

	template <typename BaseType, typename MemberType>
	AnyPointer FunctionMember<BaseType, MemberType>::Get(AnyPointer& obj)
	{
		// Make sure we have a getter.
		assert(m_getter != nullptr);

		BaseType *pObj = obj.GetPointer<BaseType>();

		// TODO: GETTING THIS VALUE LEAKS MEMORY RIGHT NOW.
		MemberType *returnValue = new MemberType((pObj->*m_getter)());
		return AnyPointer(returnValue);
	}

	template <typename BaseType, typename MemberType>
	void FunctionMember<BaseType, MemberType>::Set(AnyPointer& obj, AnyPointer& value)
	{
		// Don't do anything if we don't actually have a setter.
		if (m_setter == nullptr)
		{
			return;
		}

		BaseType *pObj = obj.GetPointer<BaseType>();
		(pObj->*m_setter)(*value.GetPointer<MemberType>());
	}

	template <typename BaseType, typename MemberType>
	Type *FunctionMember<BaseType, MemberType>::GetType()
	{
		return internal::GetType<MemberType>();
	}
}

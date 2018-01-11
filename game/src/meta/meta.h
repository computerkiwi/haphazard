/*
FILE: meta.h
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Universal.h"
#include <string>
#include <vector>
#include <unordered_map>

#include "rapidjson/document.h"

#include "lua.hpp"
#include "LuaBridge.h"
#include "Scripting/LuaEngine.h"

namespace meta
{
	class Member;

	// Helper wrappers around constructors/assignment operators. (We get pointers to these.)
	namespace internal
	{
		template <typename T>
		void DefaultConstructor(void *destination);

		template <typename T>
		void CopyConstructor(void *destination, const void *source);

		template <typename T>
		void MoveConstructor(void *destination, void *source);

		template <typename T>
		void Assignment(void *destination, const void *source);

		template <typename T>
		void MoveAssignment(void *destination, void *source);

		template <typename T>
		void Destructor(void *object);
	}

	typedef int GameObject_ID;

	class Type
	{
	public:

		typedef void(*DefaultConstructorFunction)(void *destination);
		typedef void(*CopyConstructorFunction)(void *destination, const void *source);
		typedef void(*MoveConstructorFunction)(void *destination, void *source);
		typedef void(*AssignmentFunction)(void *destination, const void *source);
		typedef void(*MoveAssignmentFunction)(void *destination, void *source);
		typedef void(*DestructorFunction)(void *object);

		typedef void(*SetGameObjectIDFunction)(void *object, GameObject_ID id);
		typedef rapidjson::Value(*SerializeFunction)(const void *object, rapidjson::Document::AllocatorType& allocator);
		typedef void(*DeserializeAssignFunction)(void *object, rapidjson::Value& jsonObject);

		Type(size_t size, const char *name,
			DefaultConstructorFunction dcf, CopyConstructorFunction ccf, MoveConstructorFunction mcf, AssignmentFunction af, MoveAssignmentFunction maf, DestructorFunction df, SerializeFunction sf,
			Type *dereferenceType)
			: m_size(size), m_name(name),
			defaultConstructor(dcf), copyConstructor(ccf), moveConstructor(mcf), assignmentOperator(af), moveAssignmentOperator(maf), destructor(df), 
			m_serializeFunction(sf), m_deserializeAssignFunction(nullptr), m_setGameObjectIDFunction(nullptr),
			m_pointerType(nullptr), m_dereferenceType(dereferenceType)
		{
			// If this fails we're trying to access a type before properly registering it.
			Assert(name != nullptr);
		}

		Type(const Type&) = delete;

		~Type();

		//---------
		// Getters
		//---------
		std::string GetName();

		size_t GetSize();

		Type *GetPointerType();

		Type *GetDereferenceType();

		Type *Type::GetDeepestDereference();

		bool IsPointerType();

		void RegisterMember(const char *name, Type *type, size_t offset);

		template <typename BaseType, typename MemberType, typename GetterType, typename SetterType>
		void RegisterMember(const char *name, GetterType getter, SetterType setter)
		{
			Assert(m_members.find(name) == m_members.end());

			m_members.emplace(name, new MemberGetSet<BaseType, MemberType>(name, getter, setter));
		}

		std::vector<Member *> GetMembers();

		// Returns nullptr if the member name doesn't exist.
		Member *Type::GetMember(const char *name);

		DefaultConstructorFunction defaultConstructor;
		CopyConstructorFunction copyConstructor;
		MoveConstructorFunction moveConstructor;
		AssignmentFunction assignmentOperator;
		MoveAssignmentFunction moveAssignmentOperator;
		DestructorFunction destructor;

		void SetSerializeFunction(SerializeFunction func)
		{
			m_serializeFunction = func;
		}
		void SetDeserializeAssignFunction(DeserializeAssignFunction func)
		{
			m_deserializeAssignFunction = func;
		}
		void SetSetGameObjectIDFunction(SetGameObjectIDFunction func)
		{
			m_setGameObjectIDFunction = func;
		}

		rapidjson::Value Serialize(const void *object, rapidjson::Document::AllocatorType& allocator);

		void DeserializeConstruct(void *objectBuffer, rapidjson::Value& jsonObject);

		void DeserializeAssign(void *object, rapidjson::Value& jsonObject);

		void SetGameObjectID(void *object, GameObject_ID id);

	private:
		size_t m_size;
		std::string m_name;

		Type *m_pointerType;
		Type *m_dereferenceType;

		std::unordered_map<std::string, Member *> m_members;

		SerializeFunction m_serializeFunction;
		DeserializeAssignFunction m_deserializeAssignFunction;
		SetGameObjectIDFunction m_setGameObjectIDFunction;
	};


	class Member
	{
	public:

		virtual std::string GetName() = 0;

		virtual Type *GetType() = 0;

		// Previous pointers received from Get are invalidated on each call to Get.
		virtual const void *Get(const void *object) = 0;

		virtual void Set(void *object, const void *data) = 0;

		virtual ~Member()
		{
		}
	};

	// Member using a simple offset.
	class MemberOffset : public Member
	{
	public:
		MemberOffset(const char *name, Type *type, size_t offset);

		//---------
		// Getters
		//---------
		virtual std::string GetName();

		virtual Type *GetType();

		virtual const void *Get(const void *object);

		virtual void *Get(void *object);

		virtual void Set(void *object, const void *data);

	private:
		std::string m_name;
		Type *m_type;
		size_t m_offset;
	};

	template <typename BaseType, typename MemberType>
	class MemberGetSet : public Member
	{
	public:
		using Getter = MemberType(BaseType::*)() const;
		using Setter = void (BaseType::*)(MemberType);

		using ReferenceGetter = MemberType&(BaseType::*)();
		using ReferenceSetter = void (BaseType::*)(const MemberType&);

		MemberGetSet(const char * name, Getter getter, Setter setter) : m_name(name), m_getter(getter), m_setter(setter), m_refGetter(nullptr), m_refSetter(nullptr)
		{
		}

		MemberGetSet(const char * name, Getter getter, ReferenceSetter setter) : m_name(name), m_getter(getter), m_setter(nullptr), m_refGetter(nullptr), m_refSetter(setter)
		{
		}

		MemberGetSet(const char * name, Getter getter, ReferenceGetter setter) : m_name(name), m_getter(getter), m_setter(nullptr), m_refGetter(nullptr), m_refSetter(nullptr)
		{
		}

		MemberGetSet(const char * name, ReferenceGetter getter, Setter setter) : m_name(name), m_getter(nullptr), m_setter(setter), m_refGetter(getter), m_refSetter(nullptr)
		{
		}

		MemberGetSet(const char * name, ReferenceGetter getter, ReferenceSetter setter) : m_name(name), m_getter(nullptr), m_setter(nullptr), m_refGetter(getter), m_refSetter(setter)
		{
		}

		MemberGetSet(const char * name, ReferenceGetter getter, ReferenceGetter setter) : m_name(name), m_getter(nullptr), m_setter(nullptr), m_refGetter(getter), m_refSetter(nullptr)
		{
		}

		//---------
		// Getters
		//---------
		virtual std::string GetName();

		virtual Type *GetType();

		virtual const void *Get(const void *object);

		virtual void Set(void *object, const void *data);

	private:
		Getter m_getter;
		Setter m_setter;
		ReferenceGetter m_refGetter;
		ReferenceSetter m_refSetter;

		MemberType m_getterBuffer; // Required so that we can return a pointer when we have a copying get function.

		std::string m_name;
	};

	class Any
	{
		friend class ::meta::Type;

		// Size the type can be before we allocate dynamic memory for it.
		static const size_t MAX_SIZE = sizeof(void *);
	public:
		template <typename T>
		Any(const T& object);

		Any(const void *object, Type *type);

		Any(const Any& other);

		Any(Any&& other);

		// Deserialize constructor
		Any(rapidjson::Value& jsonValue);

		Any& operator=(const Any& other);

		Any& operator=(Any&& other);

		~Any();

		template <typename T>
		T& GetData();

		template <typename T>
		void SetData(const T& data);

		// Dereference operator.
		Any operator*();

		bool IsNullPtr();

		//-----------------------
		// Member Getter/Setters
		//-----------------------
		
		template <typename T>
		void SetMember(Member *member, const T& value);

		void SetMember(Member *member, const Any& value);

		template <typename T>
		void SetMember(const char *memberName, const T& value);

		void SetMember(const char *memberName, const Any& value);

		template <typename T>
		T GetMember(Member *member) const;

		Any GetMember(Member *member);

		template <typename T>
		T GetMember(const char *memberName) const;

		Any GetMember(const char *memberName);

		//-------------------------------
		// Pointer Member Getter/Setters
		//-------------------------------

		template <typename T>
		void SetPointerMember(Member *member, const T& value);

		void SetPointerMember(Member *member, const Any& value);

		template <typename T>
		void SetPointerMember(const char *memberName, const T& value);

		void SetPointerMember(const char *memberName, const Any& value);

		template <typename T>
		T GetPointerMember(Member *member) const;

		Any GetPointerMember(Member *member);

		template <typename T>
		T GetPointerMember(const char *memberName) const;

		Any GetPointerMember(const char *memberName);

		//-----------------
		// General Getters
		//-----------------
		Type *GetType() const;

		//---------------
		// Serialization
		//---------------
		rapidjson::Value Serialize(rapidjson::Document::AllocatorType& allocator);

		void SetGameObjectID(GameObject_ID id);

	private:
		void *GetDataPointer();
		const void *GetDataPointer() const;

		// Points to the actual data even if we have a pointer type.
		void *GetDeepestDataPointer();
		const void *GetDeepestDataPointer() const;

	private:
		Type *m_type;
		bool m_usesPointer; // Is the data big enough that we're using dynamic memory?
		union
		{
			char m_data[MAX_SIZE];
			void *m_dataPointer;
		};
	};

	// Helpers for GetTypePointer
	namespace internal
	{
		extern std::unordered_map<std::string, Type *> typeMap;

		// For non-pointer types.
		template <typename T>
		struct TypeGetter
		{
			static Type *GetTypePointer(const char *typeName = nullptr)
			{
				static Type type(sizeof(T), typeName, internal::DefaultConstructor<T>, internal::CopyConstructor<T>, internal::MoveConstructor<T>, internal::Assignment<T>, internal::MoveAssignment<T>, internal::Destructor<T>, nullptr, nullptr);
				static bool firstCall = true;

				// Register the type in the typeMap the first time we get called.
				if (firstCall)
				{
					firstCall = false;

					Assert(typeName != nullptr);
					typeMap.insert(std::make_pair<std::string, Type *>(typeName, &type));
				}

				return &type;
			}
		};

		// For pointer types.
		template <typename T>
		struct TypeGetter<T *>
		{
			static Type *GetTypePointer(const char *typeName = nullptr)
			{
				return TypeGetter<T>::GetTypePointer(typeName)->GetPointerType();
			}
		};
	}

	template <typename T>
	Type *GetTypePointer(const char *typeName = nullptr)
	{
		return internal::TypeGetter<T>::GetTypePointer(typeName);
	}

	void Init();

	inline Type *GetTypeByName(const char *typeName)
	{
		return internal::typeMap.at(typeName);
	}

	template <typename T>
	rapidjson::Value Serialize(const T& object, rapidjson::Document::AllocatorType& allocator)
	{
		Type *type = GetTypePointer<T>();

		return type->Serialize(&object, allocator);
	}

	template <typename T>
	void DeserializeAssign(T& obj, rapidjson::Value& jsonVal)
	{
		Type *type = GetTypePointer<T>();

		type->DeserializeAssign(&obj, jsonVal);
	}

	template <typename T>
	T DeserializeConstruct(rapidjson::Value& jsonVal)
	{
		T obj;
		DeserializeAssign(obj, jsonVal);
		return obj;
	}

}

#define META_DefineType(TYPE) (::meta::GetTypePointer<TYPE>(#TYPE))

#define META_DefineMember(TYPE, MEMBER, NAME) (::meta::GetTypePointer<TYPE>()->RegisterMember(NAME, ::meta::GetTypePointer<decltype(reinterpret_cast<TYPE *>(NULL)->MEMBER)>(), offsetof(TYPE,MEMBER)));\
                                              (luabridge::getGlobalNamespace(GetGlobalLuaState()).beginClass<TYPE>(#TYPE).addData(NAME, &TYPE::MEMBER, true).endClass())

#define META_DefineSerializeFunction(TYPE, FUNCTION_PTR) (::meta::GetTypePointer<TYPE>()->SetSerializeFunction(FUNCTION_PTR))
#define META_DefineDeserializeAssignFunction(TYPE, FUNCTION_PTR) (::meta::GetTypePointer<TYPE>()->SetDeserializeAssignFunction(FUNCTION_PTR))

#define META_DefineSetGameObjectIDFunction(TYPE, FUNCTION_PTR) (::meta::GetTypePointer<TYPE>()->SetSetGameObjectIDFunction(FUNCTION_PTR))

#define META_DefineGetterSetter(BASETYPE, MEMBERTYPE, GETTER, SETTER, NAME) (::meta::GetTypePointer<BASETYPE>()->RegisterMember<BASETYPE, MEMBERTYPE>(NAME, &BASETYPE::GETTER, &BASETYPE::SETTER));\
                               (luabridge::getGlobalNamespace(GetGlobalLuaState()).beginClass<BASETYPE>(#BASETYPE).addProperty(NAME, &BASETYPE::GETTER, &BASETYPE::SETTER))

// Currently only does Lua registration.
#define META_DefineFunction(TYPE, FUNCTION, NAME) (luabridge::getGlobalNamespace(GetGlobalLuaState()).beginClass<TYPE>(#TYPE).addFunction(NAME, &TYPE::FUNCTION))

#define META_NAMESPACE(NAMESPACE)

#define META_PREREGISTER(DUMMY_TYPE) public: template <typename Dummy = int> static void Meta__Preregister__##DUMMY_TYPE()

// Preregistration function acts as a first pass to get everything setup before adding details to the types.
// Templated to allow types. Any type that has been meta registered is allowed.
#define META_REGISTER(TYPE) private: \
                                     friend class ::meta::Type; \
                             public: \
                                     static void Meta__First_Pass_Register__##TYPE() { META_DefineType(TYPE); } \
                                     template <typename Dummy = int> static void Meta__Register__##TYPE()


template <typename T>
rapidjson::Value SerializeEnum(const void *objectPtr, rapidjson::Document::AllocatorType& allocator)
{
	rapidjson::Value val;
	val.SetInt(static_cast<int>(*static_cast<const T *>(objectPtr)));

	return val;
}

template <typename T>
void DeserializeEnum(void * objectPtr, rapidjson::Value& jsonObject)
{
	T& enumObj = *reinterpret_cast<T *>(objectPtr);

	if (jsonObject.IsInt())
	{
		enumObj = static_cast<T>(jsonObject.GetInt());
	}
}

#define META_DefineEnum(ENUM_TYPE) META_DefineType(ENUM_TYPE); META_DefineSerializeFunction(ENUM_TYPE, SerializeEnum<ENUM_TYPE>); META_DefineDeserializeAssignFunction(ENUM_TYPE, DeserializeEnum<ENUM_TYPE>)

#include "meta.inl"
#include "MemberGetSet.inl"
#include "Any.inl"

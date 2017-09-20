/*
FILE: meta.h
PRIMARY AUTHOR: Kieran Williams

Copyright � 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <assert.h>

namespace meta
{
	class Member;

	// Helper wrappers around constructors/assignment operators. (We get pointers to these.)
	namespace internal
	{
		template <typename T>
		void CopyConstructor(void *destination, const void *source)
		{
			const T *sourceObject = reinterpret_cast<const T *>(source);

			new (destination) T(*sourceObject);
		}

		template <typename T>
		void MoveConstructor(void *destination, void *source)
		{
			T *sourceObject = reinterpret_cast<T *>(source);

			new (destination) T(std::move(*sourceObject));
		}

		template <typename T>
		void Assignment(void *destination, const void *source)
		{
			const T *sourceObject = reinterpret_cast<const T *>(source);
			T *destinationObject = reinterpret_cast<T *>(destination);

			*destinationObject = *sourceObject;
		}

		template <typename T>
		void MoveAssignment(void *destination, void *source)
		{
			T *sourceObject = reinterpret_cast<T *>(source);
			T *destinationObject = reinterpret_cast<T *>(destination);

			*destinationObject = std::move(*sourceObject);
		}

		template <typename T>
		void Destructor(void *object)
		{
			T *typedObject = reinterpret_cast<T *>(object);

			typedObject->~T();
		}
	}

	class Type
	{
	public:

		typedef void(*CopyConstructorFunction)(void *destination, const void *source);
		typedef void(*MoveConstructorFunction)(void *destination, void *source);
		typedef void(*AssignmentFunction)(void *destination, const void *source);
		typedef void(*MoveAssignmentFunction)(void *destination, void *source);
		typedef void(*DestructorFunction)(void *object);

		Type(size_t size, const char *name,
			CopyConstructorFunction ccf, MoveConstructorFunction mcf, AssignmentFunction af, MoveAssignmentFunction maf, DestructorFunction df,
			Type *dereferenceType)
			: m_size(size), m_name(name),
			copyConstructor(ccf), moveConstructor(mcf), assignmentOperator(af), moveAssignmentOperator(maf), destructor(df),
			m_pointerType(nullptr), m_dereferenceType(dereferenceType)
		{
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
			assert(m_members.find(name) == m_members.end());

			m_members.emplace(name, new MemberGetSet<BaseType, MemberType>(name, getter, setter));
		}

		std::vector<Member *> GetMembers();

		// Returns nullptr if the member name doesn't exist.
		Member *Type::GetMember(const char *name);

		CopyConstructorFunction copyConstructor;
		MoveConstructorFunction moveConstructor;
		AssignmentFunction assignmentOperator;
		MoveAssignmentFunction moveAssignmentOperator;
		DestructorFunction destructor;

	private:
		size_t m_size;
		std::string m_name;

		Type *m_pointerType;
		Type *m_dereferenceType;

		std::unordered_map<std::string, Member *> m_members;
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
		MemberOffset(const char *name, Type *type, size_t offset) : m_name(name), m_type(type), m_offset(offset)
		{
		}

		//---------
		// Getters
		//---------
		virtual std::string GetName()
		{
			return m_name;
		}

		virtual Type *GetType()
		{
			return m_type;
		}

		virtual const void *Get(const void *object)
		{
			return reinterpret_cast<const char *>(object) + m_offset;
		}

		virtual void *Get(void *object)
		{
			return reinterpret_cast<char *>(object) + m_offset;
		}

		virtual void Set(void *object, const void *data)
		{
			void *dataPointer = Get(object);

			m_type->assignmentOperator(dataPointer, data);
		}

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
		virtual std::string GetName()
		{
			return m_name;
		}

		virtual Type *GetType()
		{
			return GetTypePointer<MemberType>();
		}

		virtual const void *Get(const void *object)
		{
			// It's 1:30 AM and I can't be bothered to deal with this const where it shouldn't really matter. Fuck it.
			BaseType *baseObj = reinterpret_cast<BaseType *>(const_cast<void *>(object));

			if (m_getter != nullptr)
			{
				m_getterBuffer = (baseObj->*m_getter)();
				return &m_getterBuffer;
			}
			else if (m_refGetter != nullptr)
			{
				return &(baseObj->*m_refGetter)();
			}

			assert(false); // One of the above conditions should be true.
			return nullptr;
		}

		virtual void Set(void *object, const void *data)
		{
			BaseType *baseObj   = reinterpret_cast<BaseType *>(object);
			const MemberType *dataPtr = reinterpret_cast<const MemberType *>(data);

			if (m_setter != nullptr)
			{
				(baseObj->*m_setter)(*dataPtr);
				return;
			}
			else if (m_refSetter != nullptr)
			{
				(baseObj->*m_refSetter)(*dataPtr);
				return;
			}
			else if (m_refGetter != nullptr)
			{
				(baseObj->*m_refGetter)() = *reinterpret_cast<const MemberType *>(data);
				return;
			}

			assert(false); // One of the above conditions should be true.
		}

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
		// Size the type can be before we allocate dynamic memory for it.
		static const size_t MAX_SIZE = sizeof(void *);
	public:
		template <typename T>
		Any(const T& object) : m_type(GetTypePointer<T>())
		{
			if (sizeof(T) > MAX_SIZE)
			{
				m_dataPointer = new char[sizeof(T)];
				new (m_dataPointer) T(object);
				m_usesPointer = true;
			}
			else
			{
				// Copy the data into our data storage.
				new (m_data) T(object);
				m_usesPointer = false;
			}
		}

		Any(const void *object, Type *type) : m_type(type)
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

		Any(const Any& other) : m_type(other.m_type), m_usesPointer(other.m_usesPointer)
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

		Any(Any&& other) : m_type(other.m_type), m_usesPointer(other.m_usesPointer)
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

		Any& operator=(const Any& other)
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

		Any& operator=(Any&& other)
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

		~Any()
		{
			if (m_usesPointer)
			{
				m_type->destructor(m_dataPointer);
				delete[] m_dataPointer;
			}
			else
			{
				m_type->destructor(m_data);
			}
		}

		template <typename T>
		T& GetData()
		{
			assert(GetTypePointer<T>() == m_type);

			if (m_usesPointer)
			{
				return *reinterpret_cast<T *>(m_dataPointer);
			}
			else
			{
				return *reinterpret_cast<T *>(&m_data);
			}
		}

		template <typename T>
		void SetData(const T& data)
		{
			assert(GetTypePointer<T>() == m_type);

			if (m_usesPointer)
			{
				*reinterpret_cast<T *>(m_dataPointer) = data;
			}
			else
			{
				*reinterpret_cast<T *>(&m_data) = data;
			}
		}

		// Dereference operator.
		Any operator*()
		{
			assert(m_type->IsPointerType());

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

		//-----------------------
		// Member Getter/Setters
		//-----------------------
		
		template <typename T>
		void SetMember(Member *member, const T& value)
		{
			assert(!m_type->IsPointerType());
			assert(member->GetType() == GetTypePointer<T>());

			member->Set(GetDataPointer(), &value);
		}

		void SetMember(Member *member, const Any& value)
		{
			assert(!m_type->IsPointerType());
			assert(member->GetType() == value.GetType());

			member->Set(GetDataPointer(), value.GetDataPointer());
		}

		template <typename T>
		void SetMember(const char *memberName, const T& value)
		{
			assert(!m_type->IsPointerType());
			Member *member = m_type->GetMember(memberName);

			assert(member->GetType() == GetTypePointer<T>());

			member->Set(GetDataPointer(), &value);
		}

		void SetMember(const char *memberName, const Any& value)
		{
			assert(!m_type->IsPointerType());
			Member *member = m_type->GetMember(memberName);

			assert(member->GetType() == value.GetType());

			member->Set(GetDataPointer(), value.GetDataPointer());
		}

		template <typename T>
		T GetMember(Member *member) const
		{
			assert(!m_type->IsPointerType());
			assert(member->GetType() == GetTypePointer<T>());

			return *reinterpret_cast<T *>(member->Get(GetDataPointer()));
		}

		Any GetMember(Member *member)
		{
			assert(!m_type->IsPointerType());
			return Any(member->Get(GetDataPointer()), member->GetType());
		}

		template <typename T>
		T GetMember(const char *memberName) const
		{
			assert(!m_type->IsPointerType());
			Member *member = m_type->GetMember(memberName);

			assert(member->GetType() == GetTypePointer<T>());

			return *reinterpret_cast<const T *>(member->Get(GetDataPointer()));
		}

		Any GetMember(const char *memberName)
		{
			assert(!m_type->IsPointerType());
			Member *member = m_type->GetMember(memberName);

			assert(member != nullptr);

			return Any(member->Get(GetDataPointer()), member->GetType());
		}

		//-------------------------------
		// Pointer Member Getter/Setters
		//-------------------------------

		template <typename T>
		void SetPointerMember(Member *member, const T& value)
		{
			assert(m_type->IsPointerType());
			assert(member->GetType() == GetTypePointer<T>());

			member->Set(GetDeepestDataPointer(), &value);
		}

		void SetPointerMember(Member *member, const Any& value)
		{
			assert(m_type->IsPointerType());
			assert(member->GetType() == value.GetType());

			member->Set(GetDeepestDataPointer(), value.GetDataPointer());
		}

		template <typename T>
		void SetPointerMember(const char *memberName, const T& value)
		{
			assert(m_type->IsPointerType());
			Member *member = m_type->GetDeepestDereference()->GetMember(memberName);

			assert(member->GetType() == GetTypePointer<T>());

			member->Set(GetDeepestDataPointer(), &value);
		}

		void SetPointerMember(const char *memberName, const Any& value)
		{
			assert(m_type->IsPointerType());
			Member *member = m_type->GetDeepestDereference()->GetMember(memberName);

			assert(member->GetType() == value.GetType());

			member->Set(GetDeepestDataPointer(), value.GetDataPointer());
		}

		template <typename T>
		T GetPointerMember(Member *member) const
		{
			assert(m_type->IsPointerType());
			assert(member->GetType() == GetTypePointer<T>());

			return *reinterpret_cast<T *>(member->Get(GetDeepestDataPointer()));
		}

		Any GetPointerMember(Member *member)
		{
			assert(m_type->IsPointerType());
			return Any(member->Get(GetDeepestDataPointer()), member->GetType());
		}

		template <typename T>
		T GetPointerMember(const char *memberName) const
		{
			assert(m_type->IsPointerType());
			Member *member = m_type->GetDeepestDereference()->GetMember(memberName);

			assert(member->GetType() == GetTypePointer<T>());

			return *reinterpret_cast<T *>(member->Get(GetDeepestDataPointer()));
		}

		Any GetPointerMember(const char *memberName)
		{
			assert(m_type->IsPointerType());
			Member *member = m_type->GetDeepestDereference()->GetMember(memberName);

			assert(member != nullptr);

			return Any(member->Get(GetDeepestDataPointer()), member->GetType());
		}

		//-----------------
		// General Getters
		//-----------------
		Type *GetType() const
		{
			return m_type;
		}

	private:
		void *GetDataPointer()
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
		const void *GetDataPointer() const
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
		void *GetDeepestDataPointer()
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
		const void *GetDeepestDataPointer() const
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
		// For non-pointer types.
		template <typename T>
		struct TypeGetter
		{
			static Type *GetTypePointer(const char *typeName = nullptr)
			{
				static Type type(sizeof(T), typeName, internal::CopyConstructor<T>, internal::MoveConstructor<T>, internal::Assignment<T>, internal::MoveAssignment<T>, internal::Destructor<T>, nullptr);

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

}

#define META_DefineType(TYPE) (::meta::GetTypePointer<TYPE>(#TYPE))

#define META_DefineMember(TYPE, MEMBER, NAME) (::meta::GetTypePointer<TYPE>()->RegisterMember(NAME, ::meta::GetTypePointer<decltype(reinterpret_cast<TYPE *>(NULL)->MEMBER)>(), offsetof(TYPE,MEMBER)))

#define META_DefineGetterSetter(BASETYPE, MEMBERTYPE, GETTER, SETTER, NAME) (::meta::GetTypePointer<BASETYPE>()->RegisterMember<BASETYPE, MEMBERTYPE>(NAME, &BASETYPE::GETTER, &BASETYPE::SETTER))

#define META_NAMESPACE(NAMESPACE)
#define META_REGISTER(TYPE) public: static void Meta__Register__##TYPE()

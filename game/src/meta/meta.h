/*
FILE: meta.h
PRIMARY AUTHOR: Kieran Williams

Copyright � 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include <string>

namespace meta
{
	class Type;

	namespace internal
	{
		template <typename T>
		void DefaultConstructor(void *destination)
		{
			// Construct with the default constructor to the location.
			new (destination) T;
		}

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

	class Member
	{
	public:
		Member(const char *name, Type *type, size_t offset) : m_name(name), m_type(type), m_offset(offset)
		{
		}

		//---------
		// Getters
		//---------
		std::string GetName()
		{
			return m_name;
		}
		Type *GetType()
		{
			return m_type;
		}

	private:
		std::string m_name;
		Type *m_type;
		size_t m_offset;
	};

	class Type
	{
	public:

		typedef void(*DefaultConstructorFunction)(void *destination);
		typedef void(*CopyConstructorFunction)(void *destination, const void *source);
		typedef void(*MoveConstructorFunction)(void *destination, void *source);
		typedef void(*AssignmentFunction)(void *destination, const void *source);
		typedef void(*MoveAssignmentFunction)(void *destination, void *source);
		typedef void(*DestructorFunction)(void *object);

		Type(size_t size, const char *name, DefaultConstructorFunction dcf, CopyConstructorFunction ccf, MoveConstructorFunction mcf, AssignmentFunction af, MoveAssignmentFunction maf, DestructorFunction df) 
			: m_size(size), m_name(name), defaultConstructor(dcf), copyConstructor(ccf), moveConstructor(mcf), assignmentOperator(af), moveAssignmentOperator(maf), destructor(df)
		{
		}

		//---------
		// Getters
		//---------
		std::string GetName()
		{
			return m_name;
		}
		size_t GetSize()
		{
			return m_size;
		}

		DefaultConstructorFunction defaultConstructor;
		CopyConstructorFunction copyConstructor;
		MoveConstructorFunction moveConstructor;
		AssignmentFunction assignmentOperator;
		MoveAssignmentFunction moveAssignmentOperator;
		DestructorFunction destructor;

	private:
		size_t m_size;
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

		//---------
		// Getters
		//---------
		Type *GetType()
		{
			return m_type;
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

	template <typename T>
	Type *GetTypePointer(const char *typeName = nullptr)
	{
		static Type type(sizeof(T), typeName, internal::DefaultConstructor<T>, internal::CopyConstructor<T>, internal::MoveConstructor<T>, internal::Assignment<T>, internal::MoveAssignment<T>, internal::Destructor<T>);

		return &type;
	}

}

#define META_DefineType(TYPE) ::meta::GetTypePointer<TYPE>(#TYPE)

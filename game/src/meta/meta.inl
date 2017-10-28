#pragma once

#include "meta.h"

namespace meta
{
	// Helper wrappers around constructors/assignment operators. (We get pointers to these.)
	namespace internal
	{
		template <typename T>
		void DefaultConstructor(void *destination)
		{
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
}
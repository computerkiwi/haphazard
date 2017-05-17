/*******************************************************
FILE: meta.h

PRIMARY AUTHOR: Kieran Williams

PROJECT: <project_name>

Copyright 2017 DigiPen (USA) Corporation.
*******************************************************/

#pragma once

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

	private:
		
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

		// Identifier
		class TypeIdentifier
		{
		public:
			template <typename T> static unsigned int Get();
		private:
			static unsigned int CountId();
			static unsigned int _idCounter;
			unsigned int _id;
		};

		template <typename T>
		unsigned int TypeIdentifier::Get()
		{
			static bool hasRun = false;
			static int identifier = 0;

			// Generate a new id if we haven't gotten one for this type before.
			if (!hasRun)
			{
				identifier = CountId();
				hasRun = true;
			}

			return identifier;
		}

		//
		// Registration
		//
		template <typename T>
	}
}

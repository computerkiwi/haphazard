/*******************************************************
FILE: example.cpp

PRIMARY AUTHOR: Kieran Williams

PROJECT: <project_name>

DESCRIPTION: Example of everything one might want to do with the meta system.

Copyright 2017 DigiPen (USA) Corporation.
*******************************************************/

#include "example.h"
#include "meta/meta.h"
#include <iostream>

// Hide the various tests.
namespace
{
	// Doing various things with an Any object.
	void MetaBasicAnyManipulation()
	{
		// Our variable
		int foo = 5;


		// Stuff the variable into the Any.
		// This copies it - modifying the Any will not modify the original variable.
		// There's a limit on how big a value can be copied - too big will crash (by design).
		meta::Any fooCopy(foo);
		// Get the value out of the fooCopy. 
		// This does not give you a pointer or reference.
		// Make sure you use the correct template type!
		std::cout << "foo copy's value: " << fooCopy.Get<int>() << std::endl;
		// Set the value of the fooCopy. 
		// Make sure you pass it the correct type of value.
		fooCopy.Set(3);
		std::cout << "foo copy's value: " << fooCopy.Get<int>() << std::endl;
		std::cout << "foo's value: " << foo << std::endl;
		// Get a pointer to the value held inside fooCopy.
		// Again, make sure you match the type.
		// The lifetime of the value in the Any is the same as the any - be careful about the pointers.
		int *fooCopyPointer = fooCopy.GetPointer<int>();


		// Put a pointer to the variable into the Any
		meta::Any fooPointer(&foo);
		// Modifying this Any will modify the original variable.
		std::cout << "foo's value: " << foo << std::endl;
		std::cout << "Modifying fooPointer..." << std::endl;
		fooPointer.Set(500);
		std::cout << "foo's value: " << foo << std::endl;
		// Modifying the original variable will modify the Any.
		std::cout << "fooPointer's value: " << fooPointer.Get<int>() << std::endl;
		std::cout << "Modifying foo..." << std::endl;
		foo = 5;
		std::cout << "fooPointer's value: " << fooPointer.Get<int>() << std::endl;
		// Get a pointer to the original variable.
		// Again, make sure you match the type.
		int *fooPointerPointer = fooPointer.GetPointer<int>();


		// You can get the Type object from the Any.
		meta::Type *fooType = fooCopy.GetType();
		std::cout << "fooCopy's type is " << fooType->Name() << std::endl;
	}

	// Getting basic information about a type.
	void MetaBasicTypeInfo()
	{

		meta::Type *type = meta::GetTypeInfo<int>();
		std::cout << "This type's name is " << type->Name() << std::endl;
	}
}

// Function to run all the examples.
void RunMetaExamples()
{
	MetaBasicAnyManipulation();
	MetaBasicTypeInfo();
}

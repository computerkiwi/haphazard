/*******************************************************
FILE: tests.cpp

PRIMARY AUTHOR: Kieran Williams

PROJECT: <project_name>

Copyright 2017 DigiPen (USA) Corporation.
*******************************************************/

#include <iostream>
#include "meta.h"

namespace meta
{
	class ExampleA
	{
	public:
		int a;
	};
	META_REGISTER(ExampleA);

	void TestTypeIDs()
	{
		typedef int myInt;

		std::cout << "int id: " << internal::TypeIdentifier::Get<int>() << std::endl;
		std::cout << "myInt id: " << internal::TypeIdentifier::Get<myInt>() << std::endl;
		std::cout << "float id: " << internal::TypeIdentifier::Get<float>() << std::endl;
		std::cout << "ExampleA id: " << internal::TypeIdentifier::Get<ExampleA>() << std::endl;
		std::cout << "int id: " << internal::TypeIdentifier::Get<int>() << std::endl;
		std::cout << "myInt id: " << internal::TypeIdentifier::Get<myInt>() << std::endl;
		std::cout << "float id: " << internal::TypeIdentifier::Get<float>() << std::endl;
		std::cout << "ExampleA id: " << internal::TypeIdentifier::Get<ExampleA>() << std::endl;
		std::cout << "int id: " << internal::TypeIdentifier::Get<int>() << std::endl;
		std::cout << "myInt id: " << internal::TypeIdentifier::Get<myInt>() << std::endl;
		std::cout << "float id: " << internal::TypeIdentifier::Get<float>() << std::endl;
		std::cout << "ExampleA id: " << internal::TypeIdentifier::Get<ExampleA>() << std::endl;
	}

	template <typename T>
	void PrintType(T object)
	{
		std::cout
			<< "This object is of type " << meta::internal::GetType(object)->Name().c_str() << std::endl;
	}

	void TestSimpleTypeLookup()
	{
		ExampleA example;
		example.a = 5;

		PrintType(5);
		PrintType(5.0f);
		PrintType(example);
	}

	void TestAll()
	{
		TestTypeIDs();
		TestSimpleTypeLookup();
	}
}

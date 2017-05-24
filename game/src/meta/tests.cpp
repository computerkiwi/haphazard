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
		ExampleA() : a(0) {}
		int a;
		float b;
	};
	META_REGISTER(ExampleA)->RegisterProperty("a", &ExampleA::a)->RegisterProperty("b", &ExampleA::b);

	void TestTypeIDs()
	{
		typedef int myInt;

		std::cout << "int id: " << internal::TypeIdentifier::Get<int>() << std::endl;
		std::cout << "myInt id: " << internal::TypeIdentifier::Get<myInt>() << std::endl;
		std::cout << "const int id: " << internal::TypeIdentifier::Get<const int>() << std::endl;
		std::cout << "float id: " << internal::TypeIdentifier::Get<float>() << std::endl;
		std::cout << "ExampleA id: " << internal::TypeIdentifier::Get<ExampleA>() << std::endl;
	}

	template <typename T>
	void PrintType(T object)
	{
		std::cout << "This object is of type " << meta::internal::GetType(object)->Name().c_str() << std::endl;
	}

	void TestSimpleTypeLookup()
	{
		ExampleA example;
		example.a = 5;

		PrintType(5);
		PrintType(5.0f);
		PrintType(example);
	}

	void TestAny()
	{
		int x = 5;
		AnyPointer xAP(&x);
		const int *xP = xAP.GetPointer<int>();

		assert(x == *xP);
		assert(&x == xP);
	}

	void TestProperty()
	{
		ExampleA obj;
		obj.a = 3;
		AnyPointer pObj(&obj);

		std::cout << obj.a << std::endl;
		int temp = 5;
		pObj.SetProperty("a", AnyPointer(&temp));
		std::cout << obj.a << std::endl;
		assert(temp == obj.a);
		assert(&temp != &obj.a);

		const int *x = pObj.GetProperty("a").GetPointer<int>();

		assert(*x == obj.a);
	}

	void TestAll()
	{
		TestTypeIDs();
		TestSimpleTypeLookup();
		TestAny();
		TestProperty();
	}
}

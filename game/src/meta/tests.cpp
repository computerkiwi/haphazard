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
		ExampleA() : a(1), b(1.1), c(1), d(1), e(1), f(1.1), g(1.1), m_privateFloat(1.1) {}
		int a;
		float b;
		int c;
		int d;
		int e;
		float f;
		float g;

		float GetPrivateFloat() { return m_privateFloat; }
		void SetPrivateFloat(const float& value) { m_privateFloat = value; }
	private:
		float m_privateFloat;
	};
	META_REGISTER(ExampleA).
	RegisterProperty("a", &ExampleA::a).
	RegisterProperty("b", &ExampleA::b).
	RegisterProperty("c", &ExampleA::c).
	RegisterProperty("d", &ExampleA::d).
	RegisterProperty("e", &ExampleA::e).
	RegisterProperty("f", &ExampleA::f).
	RegisterProperty("g", &ExampleA::g);
	//RegisterProperty("privateFloat", &ExampleA::GetPrivateFloat, &ExampleA::SetPrivateFloat);

	std::ostream &operator<<(std::ostream& os, ExampleA& obj)
	{
		os << "---ExampleA Object---" << std::endl;
		os << "a: " << obj.a << std::endl;
		os << "b: " << obj.b<< std::endl;
		os << "c: " << obj.c << std::endl;
		os << "d: " << obj.d << std::endl;
		os << "e: " << obj.e << std::endl;
		os << "f: " << obj.f << std::endl;
		os << "g: " << obj.g << std::endl;
		os << "privateFloat: " << obj.GetPrivateFloat() << std::endl;
		os << "---------------------";

		return os;
	}

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
		PrintType(true);
		PrintType(example);
		PrintType(example.a);
	}

	void TestAny()
	{
		int x = 5;
		Any xA(x);
		const int val = xA.Get<int>();

		assert(x == val);
	}

	void TestPropertyInfo()
	{
		ExampleA obj;
		Any pObj(&obj);
		std::cout << obj << std::endl;

		std::cout << "Setting integers to 5 and floats to 3.22." << std::endl;
		
		std::vector<PropertySignature *> properties = pObj.GetType()->GetPropertiesInfo();
		for (PropertySignature *prop : properties)
		{
			Type * propType = prop->GetType();
			if (propType == meta::internal::GetType<int>())
			{
				pObj.SetProperty(prop->Name(), Any(5));
			}
			if (propType == meta::internal::GetType<float>())
			{
				pObj.SetProperty(prop->Name(), Any(3.22f));
			}
		}

		std::cout << obj << std::endl;
	}

	/*
	void TestExample()
	{
		// Anys can be:
		// -Values.
		// -Pointers

		Any anyValue = Any(5);

		int x = 5;
		Any anyPointer = Any(&x);
	}

	*/

	void TestAll()
	{
		TestTypeIDs();
		TestSimpleTypeLookup();
		TestAny();
		TestPropertyInfo();
	}
}

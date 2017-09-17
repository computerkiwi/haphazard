/*
FILE: main.cpp
PRIMARY AUTHOR: Kieran

Entry point into the program.
Currently just a place to play around in.

Copyright � 2017 DigiPen (USA) Corporation.
*/

#include "Engine/Engine.h"

#include "meta/meta.h"
#include <iostream>

struct BigInt
{
	int data;
	float moreData;
	char filler[100];
};

int main()
{
	META_DefineType(int);
	META_DefineType(float);

	META_DefineType(BigInt);
	META_DefineMember(BigInt, data, "data");
	META_DefineMember(BigInt, moreData, "moreData");


	BigInt bigInt = { 6 };
	meta::Any bigContainer = bigInt;
	std::cout << bigContainer.GetMember("moreData").GetType()->GetName() << std::endl;

	meta::Any bigPointer = &bigInt;

	bigPointer.SetPointerMember("data", 1234);

	std::cout << bigInt.data << std::endl;
	

	meta::Any movedThing = std::move(bigContainer);


	Engine engine;

	engine.MainLoop();
	// We shouldn't get past MainLoop at this point.

	return 0;
}

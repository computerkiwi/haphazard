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
	char filler[100];
};

int main()
{
	META_DefineType(int);
	META_DefineType(BigInt);


	BigInt bigInt = { 6 };
	meta::Any bigContainer = bigInt;
	std::cout << bigContainer.GetData<BigInt>().data << std::endl;

	meta::Any movedThing = std::move(bigContainer);


	Engine engine;

	engine.MainLoop();
	// We shouldn't get past MainLoop at this point.

	return 0;
}

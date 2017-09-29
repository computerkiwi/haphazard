/*
FILE: main.cpp
PRIMARY AUTHOR: Kieran

Entry point into the program.
Currently just a place to play around in.

Copyright � 2017 DigiPen (USA) Corporation.
*/

#include "Engine/Engine.h"

#include "lua.hpp"

#include "LuaBridge.h"

#include <iostream>

class TestA
{
public:
	TestA() : x(0) {}

	int x;
	void increase() { ++x; std::cout << "Increasing x to " << x << std::endl; }
};

class TestB
{
public:
	TestB(TestA &testA) : m_testA(&testA)
	{}

	TestA *m_testA;

	TestA &get()
	{
		std::cout << "Getting a TestA from a TestB" << std::endl;
		return *m_testA;
	}
};

int main()
{

	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	luabridge::getGlobalNamespace(L)

		.beginClass<TestA>("TestA")
		.addConstructor<void(*)(void)>()
		.addData<int>("x", &TestA::x, true)
		.addFunction("increase", &TestA::increase)
		.endClass()

		.beginClass<TestB>("TestB")
		.addFunction("get", &TestB::get)
		.endClass();

	int result = luaL_dofile(L, "luatest.lua");
	std::cout << "Back in C++" << std::endl;

	if (result != 0)
	{
		std::cout << lua_tostring(L, result) << std::endl;
	}

	TestA luaCreatedTest = luabridge::getGlobal(L, "MakeTestA")(5);
	std::cout << "luaCreatedTest.x : " << luaCreatedTest.x << std::endl;

	TestA testObject;

	testObject.x = 5;
	testObject.increase();
	testObject.increase();

	TestB testB(testObject);

	//luabridge::push(L, &testObject);
	//lua_setglobal(L, "testObject");

	luabridge::push(L, testB);
	lua_setglobal(L, "testB");

	int someData = 5;

	//testObject = luabridge::getGlobal(L, "testObject").cast<TestA>();
	std::cout << testObject.x << std::endl;

	std::cin.ignore();

	Engine engine;

	engine.MainLoop();
	// We shouldn't get past MainLoop at this point.

	return 0;
}

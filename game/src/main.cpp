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
	TestA() : m_x(0) {}

	void increase() { ++m_x; std::cout << "Increasing x to " << m_x << std::endl; }

	int getX() { return m_x; }

	void setX(int val) { m_x = val; }


private:
	int m_x;
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

#include "Scripting\LuaScript.h"
#include "Scripting\ScriptingUtil.h"

int main()
{
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	SetupEnvironmentTable(L);

	luabridge::getGlobalNamespace(L)

		.beginClass<TestA>("TestA")
		.addConstructor<void(*)(void)>()
		.addFunction("increase", &TestA::increase)
		.endClass()

		.beginClass<TestB>("TestB")
		.addFunction("get", &TestB::get)
		.endClass();

	LuaScript sandbox1(L, "sandboxtest.lua");
	LuaScript sandbox2(L, "sandboxtest.lua");

	

	lua_pushvalue(L, LUA_REGISTRYINDEX);
	lua_getfield(L, -1, SCRIPT_ENVIRONMENT_TABLE);

	std::cin.ignore();

	Engine engine;

	engine.MainLoop();
	// We shouldn't get past MainLoop at this point.

	return 0;
}

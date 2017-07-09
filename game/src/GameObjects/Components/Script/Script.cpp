/*
FILE: Script.cpp
PRIMARY AUTHOR: Sweet

Function Bodies for Script Component

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Script.h"


Script::Script(GameObject * parent, const char * filename) : Component(parent), mFile(filename)
{
}


const char * Script::GetFileName() const
{
	assert(mParent && "Component does not have a parent!!");
	return mFile;
}


void Script::SetFileName(const char * filename)
{
	assert(mParent && "Component does not have a parent!!");
	mFile = filename;
}


void Script::doFile(lua_State * L)
{
	luabridge::push(L, this);
	lua_setglobal(L, "this");

	if (luaL_dofile(L, mFile))
	{
		printf("%s\n", lua_tostring(L, -1));
	}
	lua_pcall(L, 0, 0, 0);
}


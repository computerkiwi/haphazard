/*
FILE: ScriptingUtil.cpp
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/

#include "Universal.h"
#include "ScriptingUtil.h"

#include <cstring>

void LuaSetUpValue(lua_State *L, const char *upValueName, int index)
{
	const char *name;
	int valueIndex = 0;

	// Find the upvalue we're looking for.
	do
	{
		++valueIndex;
		name = lua_getupvalue(L, index, valueIndex);

		if (name != nullptr)
		{
			lua_pop(L, 1);
		}

	} while (name != nullptr && strcmp(upValueName, name) != 0);

	if (name == nullptr)
	{
		Logging::Log(Logging::CORE, Logging::MEDIUM_PRIORITY, "Couldn't find Lua upvalue to be set.");
		lua_pop(L, 1);
		return;
	}

	lua_setupvalue(L, index, valueIndex);
}

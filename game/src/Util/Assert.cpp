/*
FILE: Assert.cpp
PRIMARY AUTHOR: Kieran

Deals with our own assert.

Copyright (c) 2018 DigiPen (USA) Corporation.
*/

#include "Universal.h"

#include <cassert>
#include <corecrt.h>
#include <string>
#include <Windows.h>

#include "Assert.h"
#include "Scripting/LuaScript.h"

void _HaphazardAssert(const char *expression, bool condition, const char *fileName, int line)
{
	if (!condition)
	{
		std::string errorMessage;
		errorMessage.append("An assertion has failed!\n");

		errorMessage.append("Failed Assertion: ");
		errorMessage.append(expression);

		errorMessage.push_back('\n');
		errorMessage.append("File: ");
		errorMessage.append(fileName);

		errorMessage.push_back('\n');
		errorMessage.append("Line Number: ");
		errorMessage.append(std::to_string(line));

		if (LuaScript::currentlyRunningScript)
		{
			errorMessage.push_back('\n');

			// Get the line number from Lua.
			lua_Debug lDebug;
			lua_getstack(GetGlobalLuaState(), 1, &lDebug);
			lua_getinfo(GetGlobalLuaState(), "l", &lDebug);

			errorMessage.push_back('\n');
			errorMessage.append("Lua File: ");
			errorMessage.append(LuaScript::currentFileName);

			errorMessage.push_back('\n');
			errorMessage.append("Lua Line Number: ");
			errorMessage.append(std::to_string(lDebug.currentline));
		}

		int messageReturn = MessageBox(0, errorMessage.c_str(), "Assertion Failed!", MB_ABORTRETRYIGNORE | MB_ICONWARNING);

		switch (messageReturn)
		{
		case (IDABORT):
			abort();
			break;

		case (IDRETRY):
			__debugbreak();
			break;

		case (IDIGNORE):
			// Don't do anything...
			break;

		default:
			assert(!"We shouldn't have gotten here.");
		}
	}
}

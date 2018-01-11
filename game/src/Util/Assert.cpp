/*
FILE: Assert.cpp
PRIMARY AUTHOR: Kieran

Deals with our own assert.

Copyright (c) 2018 DigiPen (USA) Corporation.
*/

#include <cassert>
#include <corecrt.h>
#include <string>

#include "Assert.h"

void _HaphazardAssert(const char *expression, bool condition, const char *fileName, int line)
{
	if (!condition)
	{
		std::string expressionStr(expression);
		std::wstring expressionStrW(expressionStr.size(), 0);
		std::copy(expressionStr.begin(), expressionStr.end(), expressionStrW.begin());

		std::string fileNameStr(fileName);
		std::wstring fileNameStrW(fileNameStr.size(), 0);
		std::copy(fileNameStr.begin(), fileNameStr.end(), fileNameStrW.begin());

		(_wassert(expressionStrW.c_str(), fileNameStrW.c_str(), (unsigned)(line)), 0);
	}
}

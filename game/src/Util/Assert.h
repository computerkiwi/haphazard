/*
FILE: Assert.h
PRIMARY AUTHOR: Kieran

Deals with our own assert.

Copyright (c) 2018 DigiPen (USA) Corporation.
*/
#pragma once

void _HaphazardAssert(const char *expression, bool condition, const char *fileName, int line);

#ifdef NDEBUG

#define Assert(expression) ((void)0)

#else

#define Assert(EXPRESSION) _HaphazardAssert(#EXPRESSION, (EXPRESSION), __FILE__, __LINE__)

#endif

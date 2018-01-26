/*
FILE: ConsoleWrite.cpp
PRIMARY AUTHOR: Kieran

Copyright (c) 2018 DigiPen (USA) Corporation.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ConsoleWrite.h"

static void DefaultWrite(const char * str, size_t length)
{
	fwrite(str, sizeof(char), length, stdout);
}

static void DefaultWriteLine()
{
	fwrite("\n", sizeof(char), 1, stdout);
	fflush(stdout);
}

static WriteFunctionPtr writeFunc = DefaultWrite;
static WriteLineFunctionPtr writeLineFunc = DefaultWriteLine;

void ConsoleLuaWrite(const char * str, size_t length)
{
	writeFunc(str, length);
}

void ConsoleLuaWriteLine()
{
	writeLineFunc();
}

void SetLuaWriteFunction(WriteFunctionPtr func)
{
	writeFunc = func;
}

void SetLuaWriteLineFunction(WriteLineFunctionPtr func)
{
	writeLineFunc = func;
}

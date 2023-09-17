/*
FILE: ConsoleWrite.h
PRIMARY AUTHOR: Kiera

Copyright (c) 2018 DigiPen (USA) Corporation.
*/
#pragma once

typedef void(*WriteFunctionPtr)(const char *str, size_t length);
typedef void(*WriteLineFunctionPtr)();

void ConsoleLuaWrite(const char *str, size_t length);

void SetLuaWriteFunction(WriteFunctionPtr func);

void ConsoleLuaWriteLine();

void SetLuaWriteLineFunction(WriteLineFunctionPtr func);

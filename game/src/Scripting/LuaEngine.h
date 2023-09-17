/*
FILE: LuaEngine.h
PRIMARY AUTHOR: Kiera

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

struct lua_State;

lua_State *GetGlobalLuaState();

void RunLuaCommand(const char *commandString);

void RunLuaCommand(const std::string& commandString);

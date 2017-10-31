/*
FILE: Settings.cpp
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "Settings.h"

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

int Settings::ScreenWidth() { return SCREEN_WIDTH; }
int Settings::ScreenHeight() { return SCREEN_HEIGHT; }
int Settings::AntiAliasing() { return 4; }
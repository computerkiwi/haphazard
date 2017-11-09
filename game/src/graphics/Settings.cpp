/*
FILE: Settings.cpp
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "Settings.h"

static int screenWidth = 800;
static int screenHeight = 600;

int Settings::ScreenWidth() { return screenWidth; }
int Settings::ScreenHeight() { return screenHeight; }
int Settings::AntiAliasing() { return 4; }

void Settings::SetScreenSize(int w, int h) { screenWidth = w; screenHeight = h; }
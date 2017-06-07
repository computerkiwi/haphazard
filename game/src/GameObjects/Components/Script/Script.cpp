/*
FILE: Script.cpp
PRIMARY AUTHOR: Sweet

Function Bodies for Script Component

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Script.h"


Script::Script(const char * filename) : mFile(filename)
{
}


const char * Script::GetFileName() const
{
	return mFile;
}


void Script::SetFileName(const char * filename)
{
	mFile = filename;
}



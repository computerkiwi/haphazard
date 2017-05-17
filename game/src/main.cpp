/*
FILE: main.cpp
PRIMARY AUTHOR: Kieran

Entry point into the program.
Currently just a place to play around in.

Copyright © 2017 DigiPen (USA) Corporation.
*/

#include <iostream>
#include "meta\tests.h"

int main()
{
	meta::TestAll();

	// Keep the console from closing.
	std::cin.ignore();

	return 0;
}

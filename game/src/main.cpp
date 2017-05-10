/*
FILE: main.cpp
PRIMARY AUTHOR: Kieran

Entry point into the program.
Currently just a place to play around in.

Copyright © 2017 DigiPen (USA) Corporation.
*/

#include <iostream>
#include "GameObjects/GameObject.h"
#include "GameObjects/Components/Sprite/Sprite.h"


int main()
{

	for (auto i = 0; i < 100; ++i)
	{
		std::cout << "Hello, world!" << std::endl;
	}


	GameObject_Space space;
	GameObject object(space);

	Sprite sprite;
	object.SetComponent(sprite);


	// Keep the console from closing.
	std::cin.ignore();

	return 0;
}

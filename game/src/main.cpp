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
	GameObject_Space space;
	space.RegisterComponentMap<Sprite>();
	
	GameObject object(space);

	std::cout << COMPONENT_GEN<Sprite>::Func << "\n";
	std::cout << COMPONENT_GEN<Sprite>::Func << "\n";

	object.SetComponent(Sprite());

	object.GetComponent<Sprite>();

	// Keep the console from closing.
	std::cin.ignore();

	return 0;
}

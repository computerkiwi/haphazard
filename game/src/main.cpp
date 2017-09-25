/*
FILE: main.cpp
PRIMARY AUTHOR: Kieran

Entry point into the program.
Currently just a place to play around in.

Copyright � 2017 DigiPen (USA) Corporation.
*/

#include <iostream>
#include "meta/tests.h"
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Engine/Quadtree/Quadtree.h"
#include <memory>
#include "Engine/Engine.h"
#include "meta/example.h"
#include "GameObjectSystem/GameSpace.h"
#include "GameObjectSystem/TextSprite.h"

#include "Util/Logging.h"

// This comment is useless.



std::ostream& operator<<(std::ostream& os, const glm::vec4& vec)
{
	os << "X: " << vec.x << std::endl;
	os << "Y: " << vec.y << std::endl;
	os << "Z: " << vec.z << std::endl;
	os << "W: " << vec.w << std::endl;

	return os;
}

int main()
{
	Engine engine;

	engine.MainLoop();
	// We shouldn't get past MainLoop at this point.

	// Keep the console from closing.
	//std::cin.ignore();

	return 0;
}

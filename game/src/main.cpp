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
#include "Engine/Engine.h"
#include "meta/example.h"
#include "GameObjectSystem/GameSpace.h"
#include "GameObjectSystem/Transform.h"
#include "GameObjectSystem/TextSprite.h"

// This comment is useless.

// GLM didnt have these, huh.
std::ostream& operator<<(std::ostream& os,const glm::mat4& matrix)
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			os << matrix[i][j];
		}

		os << std::endl;
	}

	return os;
}

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
	
	GameSpace gameSpace;
	gameSpace.registerSystem<Transform>();
	gameSpace.registerSystem<TextSprite>();

	GameObject obj(0, &gameSpace);
	obj.addComponent<Transform>();
	obj.addComponent<TextSprite>("an object");

	// Keep the console from closing.
	std::cin.ignore();

	return 0;
}

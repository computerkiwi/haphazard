/*
FILE: main.cpp
PRIMARY AUTHOR: Kieran

Entry point into the program.
Currently just a place to play around in.

Copyright � 2017 DigiPen (USA) Corporation.
*/

#include <iostream>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Engine/Quadtree.h"
#include "Engine/Engine.h"

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
	for (auto i = 0; i < 100; ++i)
	{
		std::cout << "Hello, world!" << std::endl;
	}
	
	
	Engine engine;

	luaL_openlibs(engine.GetLua());

	RegisterComponents(engine.GetLua());

	GameObject_Space space;
	space.Register<Sprite>();
	space.Register<Transform>();
	space.Register<Script>();

	GameObject object(space);
	object.SetComponent<Sprite>();
	object.SetComponent<Transform>();

	object.GetComponent<Transform>().SetScale(glm::vec2(1, 5));

	QuadTree<> tree;

	tree.AddObject(&object, object.GetComponent<Transform>().GetPosition());

	glm::mat4 matrix;
	std::cout << matrix << std::endl;

	glm::vec4 vector(1, 1, 1, 1);
	std::cout << vector << std::endl;
	glm::mat4 rotation = glm::rotate(glm::mat4(), 3.141592f, glm::vec3(0, 0, 1));
	vector = rotation * vector;
	std::cout << vector << std::endl;


	// Keep the console from closing.
	std::cin.ignore();

	return 0;
}

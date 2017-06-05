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
#include "GameObjects/GameObject.h"
#include "GameObjects/Components/Sprite/Sprite.h"

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
	
	
	GameObject_Space space;
	space.RegisterComponentMap<Sprite>();
	
	GameObject object(space);

	std::cout << COMPONENT_GEN<Sprite>::Func << "\n";
	std::cout << COMPONENT_GEN<Sprite>::Func << "\n";

	object.SetComponent(Sprite());

	object.GetComponent<Sprite>();


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

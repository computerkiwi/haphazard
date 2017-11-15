/*
FILE: Editor.cpp
PRIMARY AUTHOR: Sweet

Copyright (c) 2017 DigiPen (USA) Corporation.
*/

#include "Util.h"



std::ostream& operator <<(std::ostream& os, glm::vec2 rhs)
{
	os << rhs.x << "\t" << rhs.y;

	return os;
}


std::ostream& operator <<(std::ostream& os, glm::vec3 rhs)
{
	os << rhs.x << "\t" << rhs.y << "\t" << rhs.z;
	return os;
}


std::ostream& operator <<(std::ostream& os, glm::vec4 rhs)
{
	os << rhs.x << "\t" << rhs.y << "\t" << rhs.z << "\t" << rhs.w;
	return os;
}







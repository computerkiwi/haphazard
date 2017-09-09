/*
FILE: Transform.h
PRIMARY AUTHOR: Kieran

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

class Transform
{
public:
	Transform(float x, float y, float z, float rot) : xPos(x), yPos(y), zPos(z), rotation(rot)
	{}

	float xPos, yPos, zPos;
	float rotation;
private:

};

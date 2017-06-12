/*
FILE: Component.h
PRIMARY AUTHOR: Sweet

Entry point into the program.
Currently just a place to play around in.

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once
#include "../Component.h"


class Transform : public Component
{
public:
	Transform();

	Transform(int position, int scale);


	int GetPosition() const;
	void SetPosition(int position);

	int GetScale() const;
	void SetScale(int scale);


private:
	int mPosition;
	int mScale;
};


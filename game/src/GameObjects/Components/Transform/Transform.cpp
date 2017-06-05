/*
FILE: Component.h
PRIMARY AUTHOR: Sweet

Entry point into the program.
Currently just a place to play around in.

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once
#include "Transform.h"

Transform::Transform() : Transform(0, 1)
{
}


Transform::Transform(int position, int scale) : mPosition(position), mScale(scale)
{
}


int Transform::GetPosition() const
{
	return mPosition;
}


void Transform::SetPosition(int position)
{
	mPosition = position;
}


int Transform::GetScale() const
{
	return mScale;
}


void Transform::SetScale(int scale)
{
	mScale = scale;
}


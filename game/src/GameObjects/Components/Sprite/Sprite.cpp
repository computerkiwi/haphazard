/*
FILE: Component.h
PRIMARY AUTHOR: Sweet

Entry point into the program.
Currently just a place to play around in.

Copyright � 2017 DigiPen (USA) Corporation.
*/
#pragma once
#include "Sprite.h"


Sprite::Sprite() : Sprite(1.0f)
{
}


Sprite::Sprite(const Sprite & other) : Sprite(other.mAlpha)
{
}



Sprite::Sprite(float alpha) : mAlpha(alpha)
{
}


float Sprite::GetAlpha() const
{
	return mAlpha;
}


void Sprite::SetAlpha(float alpha)
{
	mAlpha = alpha;
}


void Sprite::ChangeAlpha(float change)
{
	mAlpha += change;
	if (mAlpha > 1)
		mAlpha = 1;
	else if (mAlpha < 0)
		mAlpha = 0;
}

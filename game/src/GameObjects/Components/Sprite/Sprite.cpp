/*
FILE: Sprite.cpp
PRIMARY AUTHOR: Sweet

Contains all the functions for Sprite

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once


#include "Sprite.h"


Sprite::Sprite() : Sprite(1.0f, nullptr)
{
}


Sprite::Sprite(const Sprite & other) : Sprite(other.mAlpha, other.mTexture)
{
}



Sprite::Sprite(float alpha, Texture texture) : mAlpha(1.0f), mTexture(texture)
{
}



Sprite::Sprite(Texture texture) : Sprite(1.0f, texture)
{
}


Sprite::Sprite(float alpha) : Sprite(alpha, nullptr)
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


Texture Sprite::GetTexture() const
{
	return mTexture;
}


void Sprite::SetTexture(Texture texture)
{
	mTexture = texture;
}


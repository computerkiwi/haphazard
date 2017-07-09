/*
FILE: Sprite.cpp
PRIMARY AUTHOR: Sweet

Contains all the functions for Sprite

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once


#include "Sprite.h"


Sprite::Sprite() : Sprite(nullptr, 1.0f, nullptr)
{
}


Sprite::Sprite(GameObject * parent) : Sprite(parent, 1.0f, nullptr)
{
}



Sprite::Sprite(GameObject * parent, const Sprite & other) : Sprite(parent, other.mAlpha, other.mTexture)
{
}



Sprite::Sprite(GameObject * parent, float alpha, Texture texture) : Component(parent), mAlpha(alpha), mTexture(texture)
{
}



Sprite::Sprite(GameObject * parent, Texture texture) : Sprite(parent, 1.0f, texture)
{
}


Sprite::Sprite(GameObject * parent, float alpha) : Sprite(parent, alpha, nullptr)
{
}



float Sprite::GetAlpha() const
{
	assert(mParent && "Component does not have a parent!!");
	return mAlpha;
}


void Sprite::SetAlpha(float alpha)
{
	assert(mParent && "Component does not have a parent!!");
	mAlpha = alpha;
}


void Sprite::ChangeAlpha(float change)
{
	assert(mParent && "Component does not have a parent!!");
	mAlpha += change;
	if (mAlpha > 1)
		mAlpha = 1;
	else if (mAlpha < 0)
		mAlpha = 0;
}


Texture Sprite::GetTexture() const
{
	assert(mParent && "Component does not have a parent!!");
	return mTexture;
}


void Sprite::SetTexture(Texture texture)
{
	assert(mParent && "Component does not have a parent!!");
	mTexture = texture;
}


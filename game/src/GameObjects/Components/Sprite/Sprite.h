/*
FILE: Sprite.h
PRIMARY AUTHOR: Sweet

Contains Sprite Definition

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once


#include "../Component.h"
#include <iostream>


using Texture = void *;

class Sprite : public Component
{
public:
	Sprite();
	explicit Sprite(GameObject * parent);

	Sprite(GameObject * parent, const Sprite & other);

	Sprite(GameObject * parent, float alpha, Texture texture);

	explicit Sprite(GameObject * parent, float alpha);
	explicit Sprite(GameObject * parent, Texture texture);

	void SetAlpha(float alpha);
	float GetAlpha() const;
	void ChangeAlpha(float change);

	Texture GetTexture() const;
	void SetTexture(Texture texture);

	void print() const
	{
		std::cout << "<Sprite alpha:" << mAlpha << " texture:" << mTexture << ">";
	}

private:
	float mAlpha;
	Texture mTexture;
};


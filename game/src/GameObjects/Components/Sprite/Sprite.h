/*
FILE: Component.h
PRIMARY AUTHOR: Sweet

Entry point into the program.
Currently just a place to play around in.

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once
#include "../Component.h"


class Sprite : public Component
{
public:
	Sprite();
	Sprite(const Sprite & other);

	explicit Sprite(float alpha);

	void SetAlpha(float alpha);
	float GetAlpha() const;
	void ChangeAlpha(float change);

private:
	float mAlpha = 10;
};


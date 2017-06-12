/*
FILE: Component.h
PRIMARY AUTHOR: Sweet

Entry point into the program.
Currently just a place to play around in.

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once


class GameObject;

class Component
{
public:
	Component() {}
	explicit Component(GameObject * parent) : mParent(parent) {}

	inline void SetParent(GameObject * parent) { mParent = parent; }
	inline GameObject * GetParent() const { return mParent; }

protected:
	GameObject * mParent = nullptr;
};



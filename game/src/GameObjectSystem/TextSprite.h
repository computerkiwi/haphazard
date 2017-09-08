/*
FILE: TextSprite.h
PRIMARY AUTHOR: Kieran

Fake "sprite" that uses a text-based interace.

Copyright © 2017 DigiPen (USA) Corporation.
*/

#pragma once

struct TextSprite
{
	TextSprite(const char *name) : m_name(name)
	{
	}

	const char * GetName() { return m_name; }

	const char *m_name;
};

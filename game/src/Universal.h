/*
FILE: Universal.h
PRIMARY AUTHOR: Kieran

General universal header containing things that will commonly be used everywhere.

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

// TODO - Make this a precompiled header.

#include "Util/Assert.h"
#include "Util/Logging.h"
#include "meta/meta.h"

constexpr unsigned long hash(const char *str)
{
	unsigned long hash = 5381;
	int c = *str;

	while (c)
	{
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
		c = *str++;
	}

	return hash;
}

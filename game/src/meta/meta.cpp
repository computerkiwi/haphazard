/*
FILE: meta.cpp
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#include "meta.h"
namespace meta
{
	namespace internal
	{
		std::unordered_map<std::string, Type *> typeMap;
	}
}
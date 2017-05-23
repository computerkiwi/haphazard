/*******************************************************
FILE: meta.cpp

PRIMARY AUTHOR: Kieran Williams

PROJECT: <project_name>

Copyright 2017 DigiPen (USA) Corporation.
*******************************************************/

#include "meta.h"

unsigned int meta::internal::TypeIdentifier::CountId()
{
	return _idCounter++;
}

unsigned int meta::internal::TypeIdentifier::_idCounter = 0;

std::map<meta::internal::TypeID, meta::Type> meta::internal::typeMap;

// Register some default types.
META_REGISTER(int);
META_REGISTER(float);

/*
FILE: BuiltInRegistration.h
PRIMARY AUTHOR: Kieran Williams

Copyright � 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "meta.h"

#define DefIntType(TYPE) META_DefineType(TYPE); META_DefineType(unsigned TYPE); META_DefineType(signed TYPE)

namespace meta
{
	class BuiltInRegistration
	{
		META_NAMESPACE(::meta);
		META_REGISTER(BuiltInRegistration)
		{
			// List taken from https://www.tutorialspoint.com/cplusplus/cpp_data_types.htm

			DefIntType(char);
			DefIntType(int);
			DefIntType(short);
			DefIntType(long);
			DefIntType(long long);

			META_DefineType(float);
			META_DefineType(double);
			META_DefineType(long double);
			META_DefineType(wchar_t);
		}
	};
}
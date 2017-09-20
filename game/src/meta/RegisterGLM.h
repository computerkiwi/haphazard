/*
FILE: BuiltInRegistration.h
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "meta.h"
#include "glm/glm.hpp"

namespace meta
{
	class RegisterGLM
	{
		META_NAMESPACE(::meta);
		META_REGISTER(RegisterGLM)
		{
			META_DefineType(glm::vec2);
			META_DefineType(glm::vec3);
			META_DefineType(glm::vec4);

			META_DefineType(glm::mat2);
			META_DefineType(glm::mat3);
			META_DefineType(glm::mat4);
		}
	};
}

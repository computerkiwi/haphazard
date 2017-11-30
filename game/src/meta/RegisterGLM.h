/*
FILE: BuiltInRegistration.h
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "meta.h"
#include "glm/glm.hpp"

static glm::vec2 AddVec2(const glm::vec2& vec1, const glm::vec2& vec2)
{
	return vec1 + vec2;
}
static glm::vec2 SubVec2(const glm::vec2& vec1, const glm::vec2& vec2)
{
	return vec1 - vec2;
}

namespace meta
{
	class RegisterGLM
	{
		META_NAMESPACE(::meta);
		META_PREREGISTER(RegisterGLM)
		{
			using namespace glm;

			META_DefineType(vec2);
			META_DefineMember(vec2, x, "x");
			META_DefineMember(vec2, y, "y");
			typedef glm::vec2(vec2::*Vec2AddFunction)(glm::vec2 *vec);
			luabridge::getGlobalNamespace(GetGlobalLuaState()).beginClass<glm::vec2>("vec2")
				.addStaticFunction("Add", AddVec2)
				.addStaticFunction("Sub", SubVec2);

			META_DefineType(glm::vec3);
			META_DefineMember(glm::vec3, x, "x");
			META_DefineMember(glm::vec3, y, "y");
			META_DefineMember(glm::vec3, z, "z");

			META_DefineType(glm::vec4);
			META_DefineMember(glm::vec4, x, "x");
			META_DefineMember(glm::vec4, y, "y");
			META_DefineMember(glm::vec4, z, "z");
			META_DefineMember(glm::vec4, w, "w");

			META_DefineType(glm::mat2);
			META_DefineType(glm::mat3);
			META_DefineType(glm::mat4);
		}
	};
}

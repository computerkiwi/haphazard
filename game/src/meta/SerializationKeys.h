/*
FILE: SerializationKeys.h
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2018 DigiPen (USA) Corporation.
*/
#pragma once

#include <vector>

const std::vector<const char *> META_TYPE_NAME =
{
	"nam",
	"meta_type_name"
};

const std::vector<const char *> META_OBJECT_DATA =
{
	"dat",
	"meta_object_data"
};

template <typename JsonType>
auto& GetMemberBackwardsCompatible(JsonType& obj, const std::vector<const char *>& compatabilityVector)
{
	for (const char *key : compatabilityVector)
	{
		if (obj.HasMember(key))
		{
			return obj[key];
		}
	}

	// Shouldn't get here.
	Assert(false);
	return obj[""];
}


/*
FILE: ResourceRegistration.cpp
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/

#include "Universal.h"
#include "ResourceManager.h"

// Resource classes to include.
#include "graphics/TextureResource.h"
#include "Scripting/ScriptResource.h"

// Once you're registered a resource type in the enum in ResourceManager.h, add to the functions below.

const char *Resource::GetFolderName(ResourceType type)
{
	// Register here:
	switch (type)
	{
	case ResourceType::TEXTURE:
		return "textures";
		break;

	case ResourceType::SCRIPT:
		return "scripts";
		break;

	default:
		Logging::Log(Logging::CORE, Logging::HIGH_PRIORITY, "Attempted to get folder path for invalid resource type.");
		return "";
		break;
	}
}

Resource * Resource::AllocateNewResource(ResourceType type, const char *folderPath, const char *fileName)
{
	// Register here:
	switch (type)
	{
	case ResourceType::TEXTURE:
		return new TextureResource(folderPath, fileName);
		break;

	case ResourceType::SCRIPT:
		return new ScriptResource(folderPath, fileName);
		break;

	default:
		Logging::Log(Logging::CORE, Logging::HIGH_PRIORITY, "Attempted to allocate resource for invalid resource type.");
		return nullptr;
		break;
	}
}


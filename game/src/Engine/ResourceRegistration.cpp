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
#include "Audio/AudioResource.h"
#include "graphics\AnimationResource.h"

#include "Engine\Engine.h"

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

	case ResourceType::SOUND:
		return "audio";
		break;

	case ResourceType::ANIMATION:
		return "animations";
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

	case ResourceType::SOUND:
		return new AudioResource(folderPath, fileName);
		break;

	case ResourceType::ANIMATION:
		return new AnimationResource(folderPath, fileName);
		break;

	default:
		Logging::Log(Logging::CORE, Logging::HIGH_PRIORITY, "Attempted to allocate resource for invalid resource type.");
		return nullptr;
		break;
	}
}


ResourceID Resource::GetDefaultResourceID(ResourceType type)
{
	switch (type)
	{
	case ResourceType::TEXTURE:
		return FilenameToID(DEFAULT_TEXTURE_NAME);

	case ResourceType::ANIMATION:
		return FilenameToID(DEFAULT_TEXTURE_NAME);

	case ResourceType::INVALID:
		assert(nullptr);
		break;

	default:
		logger.SetNextPriority(Logging::Priority::HIGH_PRIORITY);
		logger << "No valid default resource for resource type: " << GetResourceTypeName(type) << "\n";
	}

	return INVALID_TEXTURE_ID;
}


Resource *Resource::GetDefaultResource(ResourceType type)
{
	switch (type)
	{
	case ResourceType::TEXTURE:
		return engine->GetResourceManager().Get(FilenameToID(DEFAULT_TEXTURE_NAME));

	case ResourceType::ANIMATION:
		return engine->GetResourceManager().Get((DEFAULT_TEXTURE_NAME));

	case ResourceType::INVALID:
		assert(nullptr);
		break;
	default:
		logger.SetNextPriority(Logging::Priority::HIGH_PRIORITY);
		logger << "No valid default resource for resource type: " << static_cast<const char *>(GetResourceTypeName(type)) << "\n";
	}

	return nullptr;
}




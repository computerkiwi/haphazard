/*
FILE: ResourceRegistration.cpp
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/

#include "Universal.h"
#include "ResourceManager.h"

// Once you're registered a resource type in the enum in ResourceManager.h, add to the functions below.

const char *Resource::GetFolderPath(ResourceType type)
{
  // Register here:
  switch (type)
  {
  case ResourceType::TEXTURE:
    return "textures";
    break;

  default:
    Logging::Log(Logging::CORE, Logging::HIGH_PRIORITY, "Attempted to get folder path for invalid resource type.");
    return "";
    break;
  }
}

Resource * Resource::AllocateNewResource(ResourceType type, const char *filePath)
{
  // Register here:
  switch (type)
  {
  case ResourceType::TEXTURE:
    // TODO: Make this create a texture resource.
    return nullptr;
    break;

  default:
    Logging::Log(Logging::CORE, Logging::HIGH_PRIORITY, "Attempted to allocate resource for invalid resource type.");
    return nullptr;
    break;
  }
}


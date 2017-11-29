/*
FILE: ResourceManager.cpp
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/

#include "Universal.h"
#include "ResourceManager.h"
#include "graphics/TextureResource.h"
#include <experimental/filesystem>

#include "Engine/Engine.h"

void Resource::Load()
{
	Logging::Log(Logging::CORE, Logging::MEDIUM_PRIORITY, "Loading resource: ", m_folderPath, m_fileName);
	if (m_isLoaded)
	{
		return;
	}

	LoadData((m_folderPath + '\\' + m_fileName).c_str());

	m_isLoaded = true;
}

void Resource::Unload()
{
	Logging::Log(Logging::CORE, Logging::MEDIUM_PRIORITY, "Unloading resource: ", m_folderPath, m_fileName);
	if (!m_isLoaded)
	{
		return;
	}

	UnloadData();

	m_isLoaded = false;
}

void *Resource::Data()
{
	if (!m_isLoaded)
	{
		logger.SetNextChannel(Logging::CORE);
		logger << "Attempted to get unloaded resource: " << m_folderPath << m_fileName << "\n";
		return nullptr;
	}

	return GetData();
}

ResourceID Resource::FilenameToID(const char *fileName)
{
	return hash(fileName);
	// Really shitty hash.
	// TODO: Make the hash less shitty.
	/*size_t hash = 0;
	size_t len = strlen(fileName);
	for (size_t i = 0; i < len; ++i)
	{
	hash += fileName[i] * fileName[i];
	}

	return hash;*/
}

ResourceManager::~ResourceManager()
{
	// Unload and delete each resource.
	for (auto resourcePair : m_resources)
	{
		Resource *res = resourcePair.second;
		if (res->IsLoaded())
		{
			res->Unload();
		}
		delete res;
	}
}

ResourceManager & ResourceManager::operator=(const ResourceManager & other)
{
	m_resources = other.m_resources;

	return *this;
}

void ResourceManager::Init()
{
	using namespace std::experimental;

	// Go through each resource type folder.
	for (ResourceType resType = static_cast<ResourceType>(0); resType < ResourceType::COUNT; resType = static_cast<ResourceType>(static_cast<int>(resType) + 1))
	{
		std::string directoryPathString = std::string(ASSETS_FOLDER) + "/" + Resource::GetFolderName(resType) + "/";

		// Get all the files from it.
		for (auto iter : filesystem::directory_iterator(directoryPathString.c_str()))
		{
			// Extract info about the file.
			std::string fileName;
			fileName = iter.path().filename().string();
			std::string folderDir;
			folderDir = iter.path().parent_path().string();
			ResourceID fileID = Resource::FilenameToID(fileName.c_str());

			// Make sure we don't already have something with that ID.
			auto resPair = m_resources.find(fileID);
			if (resPair != m_resources.end())
			{
				Logging::Log(Logging::CORE, Logging::HIGH_PRIORITY, "Could not place file \"", fileName, "\" in resource map. ID already taken by file \"", resPair->second->FileName(), '"');
				continue;
			}

			// Make an actual resource.
			Resource *resource = Resource::AllocateNewResource(resType, folderDir.c_str(), fileName.c_str());
			if (resource == nullptr)
			{
				Logging::Log(Logging::CORE, Logging::HIGH_PRIORITY, "Could not create resource for file \"", fileName, "\".");
				continue;
			}

			// Insert the resource into the map.
			m_resources.insert(std::make_pair(fileID, resource));
		}
	}
}

void ResourceManager::Refresh()
{
	// This function is not yet supported!
	assert(false);
}

void ResourceManager::LoadAll()
{
	for (auto& resPair : m_resources)
	{
		Resource& res = *resPair.second;
		if (!res.IsLoaded())
		{
			res.Load();
		}
	}
}

void ResourceManager::GetResourcesOfType(ResourceType type, std::vector<Resource*>& vec)
{
	for (auto& resPair : m_resources)
	{
		Resource& res = *resPair.second;
		if (res.GetResourceType() == type)
		{
			vec.push_back(&res);
		}
	}
}

std::vector<Resource*> ResourceManager::GetResourcesOfType(ResourceType type)
{
	std::vector<Resource*> vec;
	GetResourcesOfType(type, vec);
	return vec;
}

std::vector<Resource *> ResourceManager::GetResourcesOfType_Alphabetical(ResourceType type)
{
	std::vector<Resource *> vec;
	GetResourcesOfType(type, vec);

	std::sort(vec.begin(), vec.end(),
	
	[](Resource *first, Resource *second) -> bool
	{
		return first->GetFilename() < second->GetFilename();
	});

	return vec;
}


Resource *ResourceManager::Get(ResourceID id)
{
	// Search in the map for the resource and return it if it exists.
	auto resIter = m_resources.find(id);

	if (resIter == m_resources.end())
	{
		return nullptr;
	}
	else
	{
		return resIter->second;
	}
}

Resource *ResourceManager::Get(const char *fileName)
{
	return Get(Resource::FilenameToID(fileName));
}

ResourceManager& ResourceManager::GetManager()
{
	return engine->GetResourceManager();
}

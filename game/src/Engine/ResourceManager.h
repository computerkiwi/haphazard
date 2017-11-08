/*
FILE: ResourceManager.h
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Universal.h"
#include <map>
#include <string>
#include <vector>

// New resource types must be registered here and in ResourceRegistration.cpp
enum class ResourceType : int
{
	INVALID = -1,
	TEXTURE,
	SCRIPT,
	SOUND,
	COUNT
};

typedef size_t ResourceID;

// Abstract class for resources.
class Resource
{
public:

	void Load();
	void Unload();

	void *Data();

	bool IsLoaded() { return m_isLoaded; }
	std::string FileName() { return m_fileName; }
	ResourceID Id() { return m_id; }
	ResourceType GetResourceType() { return GetType(); }

	static ResourceID FilenameToID(const char *fileName);

	// Registration functions.
	static Resource *AllocateNewResource(ResourceType type, const char *folderPath, const char *fileName);
	static const char *GetFolderName(ResourceType type);

protected:
	Resource(const char *folderPath, const char *fileName) : m_folderPath(folderPath), m_fileName(fileName), m_isLoaded(false)
	{
		m_id = FilenameToID(fileName);
	}

private:
	virtual void LoadData(const char *filePath) = 0;
	virtual void UnloadData() = 0;
	virtual void *GetData() = 0;
	virtual ResourceType GetType() = 0;

	std::string m_folderPath; // File path excluding the file name.
	std::string m_fileName;
	ResourceID m_id;
	bool m_isLoaded;
};

class ResourceManager
{
	const char * const ASSETS_FOLDER = "assets";
public:
	~ResourceManager();
	ResourceManager& operator=(const ResourceManager& other);

	void Init();

	void Refresh();

	void LoadAll();

	void GetResourcesOfType(ResourceType type, std::vector<Resource *>& vec);
	std::vector<Resource *> GetResourcesOfType(ResourceType type);

	Resource *Get(ResourceID id);
	Resource *Get(const char *fileName);

	static ResourceManager& GetManager();

private:
	std::map<ResourceID, Resource *> m_resources;

	META_REGISTER(ResourceManager)
	{
		META_DefineType(ResourceManager);

		// TODO: Make a proper static function meta registration.
		luabridge::getGlobalNamespace(GetGlobalLuaState()).beginClass<ResourceManager>("ResourceManager").addStaticFunction("GetManager", &ResourceManager::GetManager).endClass();
	}
};

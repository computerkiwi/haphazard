/*
FILE: ResourceManager.h
PRIMARY AUTHOR: Kiera

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "Universal.h"
#include <map>
#include <string>
#include <vector>
#include <algorithm>


#define DEFAULT_TEXTURE_NAME "default.png"



// New resource types must be registered here and in ResourceRegistration.cpp
enum class ResourceType : int
{
	INVALID = -1,
	TEXTURE,
	SCRIPT,
	SOUND,
	ANIMATION, // make AnimationResource that derives Resource and do the thing at ResourceRegistration.cpp
	COUNT
};


constexpr char *ResourceTypeNames[] =
{
	"Texture",
	"Script",
	"Sound",
	"Animation"
};



typedef size_t ResourceID;

// Abstract class for resources.
class Resource
{
public:

	void Load();
	void Unload();

	void *Data();

	void Reload();

	bool IsLoaded() { return m_isLoaded; }
	std::string FileName() { return m_fileName; }
	const std::string& GetFilename() const { return m_fileName; }
	ResourceID Id() { return m_id; }
	ResourceType GetResourceType() { return GetType(); }

	static ResourceID FilenameToID(const char *fileName);

	// Returns default source for a type
	static Resource *GetDefaultResource(ResourceType type);
	static ResourceID GetDefaultResourceID(ResourceType type);

	static const char *GetResourceTypeName(ResourceType type) 
	{
		if (static_cast<int>(type) == -1)
		{
			return "Invalid";
		}

		return ResourceTypeNames[static_cast<int>(type)]; 
	}

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
	virtual void ReloadData(const char *filePath) {/* Do nothing by default for backwards compatability reasons */ }

	std::string m_folderPath; // File path excluding the file name.
	std::string m_fileName;
	ResourceID m_id;
	bool m_isLoaded;

	// Don't register it properly because it's abstract aka not real.
	META_PREREGISTER(Resource)
	{
		luabridge::getGlobalNamespace(GetGlobalLuaState()).beginClass<Resource>("Resource").addStaticFunction("FilenameToID", FilenameToID);
	}
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
	std::vector<Resource *> GetResourcesOfType_Alphabetical(ResourceType type);

	template <typename... Types>
	std::vector<Resource *> GetResourcesOfTypes_Alphabetical(Types&&... types)
	{
		std::vector<Resource *> vec;

		using expander = int[];
		expander{ (static_cast<void>(GetResourcesOfType(std::forward<Types>(types), vec)), 0)... };

		std::sort(vec.begin(), vec.end(),

			[](Resource *first, Resource *second) -> bool
		{
			return first->GetFilename() < second->GetFilename();
		});

		return vec;
	}

	Resource *Get(ResourceID id);
	Resource *Get(const char *fileName);

	static ResourceManager& GetManager();

private:
	std::map<ResourceID, Resource *> m_resources;

	META_REGISTER(ResourceManager)
	{
		// TODO: Make a proper static function meta registration.
		luabridge::getGlobalNamespace(GetGlobalLuaState()).beginClass<ResourceManager>("ResourceManager").addStaticFunction("GetManager", &ResourceManager::GetManager).endClass();
	}
};

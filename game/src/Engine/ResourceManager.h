/*
FILE: ResourceManager.h
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include <string>

// New resource types must be registered here and in ResourceRegistration.cpp
enum class ResourceType : int
{
  INVALID = -1,
  TEXTURE,
  COUNT
};

// Abstract class for resources.
class Resource
{
public:
  
  void Load();
  void Unload();

  void *Data();

  bool IsLoaded() { return m_isLoaded; }
  std::string FileName() { return m_fileName; }

  // Registration functions.
  static Resource *AllocateNewResource(ResourceType type, const char *filePath);
  static const char *GetFolderPath(ResourceType type);

protected:
  Resource(const char *folderPath, const char *fileName) : m_folderPath(folderPath), m_fileName(fileName), m_isLoaded(false)
  {
  }

private:
  virtual void LoadData(const char *filePath) = 0;
  virtual void UnloadData() = 0;
  virtual void *GetData() = 0;
  virtual ResourceType GetType() = 0;

  std::string m_folderPath; // File path excluding the file name.
  std::string m_fileName;
  bool m_isLoaded;
};

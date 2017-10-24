/*
FILE: ResourceManager.cpp
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/

#include "Universal.h"
#include "ResourceManager.h"

void Resource::Load()
{
  Logging::Log(Logging::CORE, Logging::MEDIUM_PRIORITY, "Loading resource: ", m_folderPath, m_fileName);
  if (m_isLoaded)
  {
    return;
  }

  LoadData((m_folderPath + m_fileName).c_str());

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
    Logging::Log(Logging::CORE, Logging::MEDIUM_PRIORITY, "Attempted to get unloaded resource: ", m_folderPath, m_fileName);
    return nullptr;
  }

  return GetData();
}

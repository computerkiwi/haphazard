/*
FILE: ScriptResource.h
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include <string>

#include "Engine/ResourceManager.h"

class ScriptResource : public Resource
{
	// Inherited via Resource
	virtual void LoadData(const char * filePath) override;
	virtual void UnloadData() override;
	virtual void *GetData() override; // Returns a std::string ptr.
	virtual ResourceType GetType() override;

public:
	ScriptResource(const char *folderPath, const char *fileName);

private:
	std::string m_fileString;
};

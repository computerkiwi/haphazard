/*
FILE: ScriptSystem.h
PRIMARY AUTHOR: Kiera

Copyright (c) 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "GameObjectSystem\GameSpace.h"


struct lua_State;

class ScriptSystem : public SystemBase
{
public:
	virtual void Init() override;
	virtual void Update(float dt) override;
	virtual std::size_t DefaultPriority() override;

	// Inherited via SystemBase
	virtual SystemBase * NewDuplicate() override;

	static void ReloadAll();
private:
	static bool shouldReloadAllScripts;
};

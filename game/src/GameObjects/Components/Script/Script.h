/*
FILE: Script.h
PRIMARY AUTHOR: Sweet

Definition for the Script Component

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "../Component.h"
#include "../../../Engine/Engine.h"

class Script : public Component
{
public:
	explicit Script(GameObject * parent, const char * filename);

	const char * GetFileName() const;
	void SetFileName(const char * filename);

	void doFile(lua_State * L);

	template <typename T>
	T & GetComponent()
	{
		assert(mParent && "Component does not have a parent!!");
		return mParent->GetComponent<T>();
	}

	template <typename T>
	void SetComponent(T & component)
	{
		mParent->SetComponent(component);
	}


private:
	const char * mFile = nullptr;
};


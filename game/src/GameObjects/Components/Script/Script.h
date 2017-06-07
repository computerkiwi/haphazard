/*
FILE: Script.h
PRIMARY AUTHOR: Sweet

Definition for the Script Component

Copyright © 2017 DigiPen (USA) Corporation.
*/
#pragma once

#include "../Component.h"


class Script : public Component
{
public:
	explicit Script(const char * filename);

	const char * GetFileName() const;
	void SetFileName(const char * filename);

private:
	const char * mFile;
};


/*******************************************************
FILE: example.h

PRIMARY AUTHOR: Kieran Williams

PROJECT: <project_name>

DESCRIPTION: Example of everything one might want to do with the meta system.

Copyright 2017 DigiPen (USA) Corporation.
*******************************************************/
#pragma once

#include "meta/meta.h"

class MetaExampleClassA
{
public:
	int publicVar;

	float getPrivateVar() const { return m_privateVar; }
	void setPrivateVar(float value) { m_privateVar = value; }

private:
	float m_privateVar;

	// Allows determining type with inheritance.
	// Note the lack of semicolon.
	META_VIRTUAL_DECLARE(MetaExampleClassA)
};



// Function to run all the examples.
void RunMetaExamples();

/*
FILE: EditorScript.h
PRIMARY AUTHOR: Kieran

Displaying individual Lua scripts in the editor.

Copyright (c) 2018 DigiPen (USA) Corporation.
*/

#pragma once

#include "Scripting/LuaScript.h"
#include "Scripting/ScriptComponent.h"
#include "Editor.h"

// Returns false if the script is removed.
bool ImGui_IndividualScript(LuaScript & script, ScriptComponent * script_c, GameObject object, Editor * editor);

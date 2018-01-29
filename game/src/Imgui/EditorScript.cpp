/*
FILE: EditorScript.cpp
PRIMARY AUTHOR: Kieran

Displaying individual Lua scripts in the editor.

Copyright (c) 2018 DigiPen (USA) Corporation.
*/

#include "EditorScript.h"
#include "Engine/Engine.h"
#include "Type_Binds.h"

using namespace ImGui;

static const size_t STRING_BUFFER_SIZE = 2048;

static void HandleStringVar(LuaScript &script, const char *name, const std::string& value)
{
	Assert(value.size() < STRING_BUFFER_SIZE);

	// Setup a buffer and copy our string into it.
	char string_buffer[STRING_BUFFER_SIZE];
	char *stringEnd = std::copy(value.begin(), value.end(), string_buffer);
	*(stringEnd) = 0;

	// If the user edits the string, update it in the script.
	if (InputText(name, string_buffer, STRING_BUFFER_SIZE, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		script.SetVar(name, meta::Any(std::string(string_buffer)));
	}
}

static void HandleNumberVar(LuaScript &script, const char *name, lua_Number value)
{
	float valueBuffer = value;

	if (InputFloat(name, &valueBuffer))
	{
		script.SetVar(name, meta::Any(static_cast<lua_Number>(valueBuffer)));
	}
}

static void HandleBoolVar(LuaScript &script, const char *name, bool value)
{
	bool valueBuffer = value;

	if (Checkbox(name, &valueBuffer))
	{
		script.SetVar(name, meta::Any(valueBuffer));
	}
}

static void HandleVar(LuaScript &script, std::pair<std::string, meta::Any>& var)
{
	meta::Type *type = var.second.GetType();

	if (type == meta::GetTypePointer<std::string>())
	{
		HandleStringVar(script, var.first.c_str(), var.second.GetData<std::string>());
	}
	else if (type == meta::GetTypePointer<lua_Number>())
	{
		HandleNumberVar(script, var.first.c_str(), var.second.GetData<lua_Number>());
	}
	else if (type == meta::GetTypePointer<bool>())
	{
		HandleBoolVar(script, var.first.c_str(), var.second.GetData<bool>());
	}
}

// Returns false if the script is removed.
bool ImGui_IndividualScript(LuaScript &script, ScriptComponent *script_c, GameObject object, Editor *editor)
{
	ResourceManager& rm = engine->GetResourceManager();
	std::string fileName = rm.Get(script.GetResourceID())->FileName();

	// Copy the fileName into the header without '.lua'
	std::string headerName(fileName.size(), 0);
	std::copy(fileName.begin(), fileName.end() - 4, headerName.begin());
	headerName += "##script_";
	headerName += fileName;

	if (CollapsingHeader(headerName.c_str()))
	{
		// Button to remove the script.
		std::string	removeButtonName = "Remove##script_remove_";
		removeButtonName += fileName;
		if (Button(removeButtonName.c_str()))
		{
			editor->Push_Action({ 0, script, nullptr,{ object, true },
				[](EditorAction& a)
			{
				ComponentHandle<ScriptComponent> handle(a.handle);

				if (handle.GetGameObject().IsValid())
				{
					if (a.redo)
					{
						ResourceID id = a.current.GetData<LuaScript>().GetResourceID();
						std::vector<LuaScript>& scripts = handle->scripts;

						for (size_t i = 0; i < scripts.size(); i++)
						{
							LuaScript& script = scripts[i];

							if (script.GetResourceID() == id)
							{
								scripts.erase(handle->scripts.begin() + i);
							}
						}
					}
					else
					{
						handle->scripts.emplace_back(a.current.GetData<LuaScript>());
					}
				}
			}
			});

			// Remove the script and return that information.
			script_c->RemoveScript(script);
			return false;
		}

		SameLine();
		std::string reloadButtonName = "Reload##script_reload_";
		reloadButtonName += fileName;
		if (Button(reloadButtonName.c_str()))
		{
			script.Reload();
		}

		SameLine();
		std::string resetButtonName = "Reset##script_reset_";
		resetButtonName += fileName;
		if (Button(resetButtonName.c_str()))
		{
			script.Reset();
		}

		// Get manipulators for all the variables.
		std::vector<std::pair<std::string, meta::Any>> vars = script.GetAllVars();
		typedef std::pair<std::string, meta::Any> VarPair;
		std::sort(vars.begin(), vars.end(), [](const VarPair& a, const VarPair& b) {return a.first < b.first; });
		for (auto& var : vars)
		{
			HandleVar(script, var);
		}
	}

	// We didn't remove this script.
	return true;
}

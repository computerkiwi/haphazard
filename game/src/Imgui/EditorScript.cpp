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
	Assert(value.size() >= STRING_BUFFER_SIZE);

	// Setup a buffer and copy our string into it.
	char string_buffer[STRING_BUFFER_SIZE];
	std::copy(value.begin(), value.end(), string_buffer);

	// If the user edits the string, update it in the script.
	if (InputText(name, string_buffer, STRING_BUFFER_SIZE, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		script.SetVar(name, meta::Any(std::string(string_buffer)));
	}
}

static void HandleVar(LuaScript &script, std::pair<std::string, meta::Any>& var)
{
	meta::Type *type = var.second.GetType();

	if (type == meta::GetTypePointer<std::string>())
	{
		HandleStringVar(script, var.first.c_str(), var.second.GetData<std::string>());
	}
}

void ImGui_IndividualScript(LuaScript &script, ScriptComponent *script_c, GameObject object, Editor *editor)
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

			script_c->RemoveScript(script);
			return;
		}

		// Get manipulators for all the variables.
		std::vector<std::pair<std::string, meta::Any>> vars = script.GetAllVars();
		for (auto& var : vars)
		{
			HandleVar(script, var);
		}
	}
}

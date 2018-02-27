/*
FILE: MultiselectUndo.h
PRIMARY AUTHOR: Kieran

Class that can be used to handle undo on multiselect objects.

Copyright (c) 2018 DigiPen (USA) Corporation.
*/
#pragma once

#include "Editor.h"

class MultiselectTransformUndo
{
public:
	static EditorAction CreateAction(const EditorAction& mainAction, const std::vector<std::pair<GameObject, glm::vec2>>& multiselectList)
	{
		MultiselectTransformUndo undo(mainAction, multiselectList);
		EditorAction newAction = { 0, 0 };

		newAction.current = undo;
		newAction.redo = false;
		newAction.func = [](EditorAction& action)
		{
			MultiselectTransformUndo multi = action.current.GetData<MultiselectTransformUndo>();

			// Call the internal undo/redo for the base object.
			multi.m_mainAction.redo = action.redo;
			multi.m_mainAction.func(multi.m_mainAction);

			// Update positions.
			GameObject baseObj = multi.m_mainAction.handle.id;
			ComponentHandle<TransformComponent> baseTransform = baseObj.GetComponent<TransformComponent>();
			glm::vec2 basePos = baseTransform->GetPosition();
			for (const auto& objPair : multi.m_multiselectList)
			{
				ComponentHandle<TransformComponent> objTransform = objPair.first.GetComponent<TransformComponent>();
				objTransform->SetPosition(objPair.second + basePos);
			}

		};

		return newAction;
	}

	MultiselectTransformUndo() : m_mainAction({ 0, 0 })
	{
		Assert(false && "Don't use the default constructor for MultiselectTransformUndo");
	}
	
	private:


	MultiselectTransformUndo(const EditorAction& mainAction, const std::vector<std::pair<GameObject, glm::vec2>>& multiselectList) : m_mainAction(mainAction), m_multiselectList(multiselectList)
	{}

	EditorAction m_mainAction;
	std::vector<std::pair<GameObject, glm::vec2>> m_multiselectList;

	META_REGISTER(MultiselectTransformUndo)
	{
	}
};

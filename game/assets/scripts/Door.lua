--[[
FILE: Door.lua
PRIMARY AUTHOR: Ash Duong

Copyright (c) 2017 DigiPen (USA) Corporation.
]]--

DESTINATION_LEVEL		= "MainMenu.json"	 

function GoToNextLevel()
	Engine.LoadLevel(DESTINATION_LEVEL)
end

function StartTransition()

	transitionStarted = true

	transitionScreen = GameObject.LoadPrefab("assets/prefabs/level_transition/LevelTransition.json")

end

function OnCollisionEnter(other)
	if(other:HasTag("Player") and transitionStarted == false)
	then
		if(other:GetScript("InputHandler.lua").jumpPressed)
		then
			transitionStarted = true
			StartTransition(NewLevelTransition(DESTINATION_LEVEL))
		end
	end
end

-----------------------------------------------------------------------
-- TRANSITION CODE

-- (mostly stolen from PauseMenu.lua)

local TRANSITION_ACTION

function NewLevelTransition(level)
  return function()
    Engine.LoadLevel(level)
  end
end

local transitionTime = 1.8

transitionStarted = false

transitionScreen = nil

function Update(dt)
	if(transitionStarted)
	then
		transitionTime = transitionTime - dt

		transitionScreen:GetTransform().position = ScreenToWorld(vec2(0,0))

		if(transitionTime < 0)
		then
			TRANSITION_ACTION()
		end
	end
end

function StartTransition(transitionAction)
  TRANSITION_ACTION = transitionAction
  transitionStarted = true

  transitionScreen = GameObject.LoadPrefab("assets/prefabs/level_transition/LevelTransitionFast.json")

  transitionScreen:GetTransform().position = ScreenToWorld(vec2(0,0))
end

--[[
FILE: Credits.lua
PRIMARY AUTHOR: Brett Schiff

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

_G.spaceScrollMultiplier = 8

timeToLeave = 3
currTime = 0

-----------------------------------------------------------------------
-- TRANSITION CODE

-- (mostly stolen from EndLevelScript.lua)

local TRANSITION_ACTION

function ContinuePressed()
	-- Spacebar
	if (IsHeld(KEY.Space))
	then
		return true
	end
	
	-- A Button on the gamepads
	for player = 0,3
	do
		local A_BUTTON = 0
		if (GamepadIsHeld(player, A_BUTTON))
		then
			return true
		end
	end
	
	-- Failed to find a continue.
	return false
end

function NewLevelTransition(level)
  return function()
    Engine.LoadLevel(level)
  end
end

local transitionTime = 1.8

transitionStarted = false

transitionScreen = nil
transitionSparkles = nil

function UpdateLevelTransition(dt)
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

-----------------------------------------------------------------------

function Update(dt)

	if(ContinuePressed())
	then

		currTime = currTime + (dt * _G.spaceScrollMultiplier)

	else

		currTime = currTime + dt

	end

	if (transitionStarted)
	then

		UpdateLevelTransition(dt)

	elseif(currTime >= timeToLeave)
	then

		-- go to the main menu
		StartTransition(NewLevelTransition("MainMenu.json"))

	end

end

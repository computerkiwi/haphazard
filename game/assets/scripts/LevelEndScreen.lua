--[[
FILE: LevelEndScreen.lua
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

function Start()
  -- Initialize the end screen table if it doesn't exist yet.
  _G.globalEndScreenTable = _G.globalEndScreenTable or {nextLevel = "defaultLevel.json"}
end

function ContinuePressed()
	-- Spacebar
	if (OnPress(KEY.Space))
	then
		return true
	end
	
	-- A Button on the gamepads
	for player = 0,3
	do
		local A_BUTTON = 0
		if (GamepadOnPress(player, A_BUTTON))
		then
			return true
		end
	end
	
	-- Failed to find a continue.
	return false
end

function Update(dt)	
  -- TODO: Check/show stats.
  if (ContinuePressed())
  then
    Engine.LoadLevel(_G.globalEndScreenTable.nextLevel)
  end
end

--[[
FILE: GemScript.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

WIN_LEVEL = "YouWinLevel.json"

function OnCollisionEnter(other)
  -- Player takes gem
  if (other:HasTag("Player"))
  then
		_G.globalEndScreenTable.nextLevel = WIN_LEVEL
		Engine.LoadLevel("LevelEndScreen.json")
  end
  
end

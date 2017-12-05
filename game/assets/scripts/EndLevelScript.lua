--[[
FILE: GemScript.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

NUM_GEMS_TO_WIN = 3
WIN_LEVEL = "YouWinLevel.json"

function OnCollisionEnter(other)
  -- Player takes gem
  if (other:HasTag("Player"))
  then
    local gems = GameObject.FindByName("Stats"):GetScript("PlayerStats.lua").GetGems()
	if(gems == NUM_GEMS_TO_WIN)
	then
		Engine.LoadLevel(WIN_LEVEL)
	end
  end
  
end

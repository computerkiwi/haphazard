--[[
FILE: GemScript.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

function OnCollisionEnter(other)
  -- Player takes gem
  if (other:HasTag("Player"))
  then
	PlaySound("important_pickup.mp3", 0.4, 1, false)
    print("Gem collected")
    GameObject.FindByName("Stats"):GetScript("PlayerStats.lua").AddGem()
	
    -- Deactivate the object
    this:Deactivate()
  end
end

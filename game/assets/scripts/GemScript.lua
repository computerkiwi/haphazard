--[[
FILE: GemScript.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

function OnCollisionEnter(other)
  -- Player takes gem
  if (other:HasTag("Player"))
  then
    print("Gem collected")
    GameObject.FindByName("Stats"):GetScript("PlayerStats.lua").AddGem()
	
    -- Deactivate the object
    this:Deactivate()
  end
end

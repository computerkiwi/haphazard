--[[
FILE: GemScript.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

function OnCollisionEnter(other)
  -- Player takes gem
  if (other:HasTag("Player"))
  then
	PlaySound("Coin6.wav", 1, 1, false)
    print("Gem collected")
	
    -- Set gnome to have gem
    other:GetScript("GnomeStatus.lua").hasGem = true

    -- Deactivate the object
    this:Deactivate()
  end
end

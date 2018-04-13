--[[
FILE: GemScript.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

KeyID = "DefaultGem"

function OnCollisionEnter(other)
  -- Player takes gem
  if (other:HasTag("Player") and other:GetScript("GnomeStatus.lua").hasGem == false)
  then
	PlaySound("Coin6.wav", 1, 1, false)
    print("Gem collected")
	
    -- Set gnome to have gem
    other:GetScript("GnomeStatus.lua").hasGem = true
	other:GetScript("GnomeStatus.lua").GemID = KeyID

    -- Deactivate the object
    this:Deactivate()
  end
end

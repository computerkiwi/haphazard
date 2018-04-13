--[[
FILE: GemScript.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

KeyID = "DefaultGem"

local thisCollected = false
function OnCollisionEnter(other)
  -- Player takes gem
  if (not thisCollected and other:HasTag("Player") and other:GetScript("GnomeStatus.lua").hasGem == false)
  then
		PlaySound("Coin6.wav", 1, 1, false)
		thisCollected = true
	
    -- Set gnome to have gem
		other:GetScript("GnomeStatus.lua").hasGem = true
		other:GetScript("GnomeStatus.lua").GemID = KeyID

    -- Deactivate the object
    this:Deactivate()
  end
end

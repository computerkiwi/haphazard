--[[
FILE: CoinScript.lua
PRIMARY AUTHOR: Lya Vera

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

function Start()
  this:Activate()
end -- fn end

function OnCollisionEnter(other)
  -- Player takes coin
  if (other:HasTag("Player"))
  then

  
  --[[

	PlaySound("regular_pickup.mp3", 0.2, 1, false)

    local score = GameObject.FindByName("Stats")

    -- Increment score
    local script = score:GetScript("PlayerStats.lua")
    script.AddScore()

  ]]

    -- Deactivate the object
    this:Deactivate()
	PlaySound("regular_pickup.mp3", 0.2, 1, false)
  end
end -- fn end
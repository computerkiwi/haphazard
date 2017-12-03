--[[
FILE: CoinScript.lua
PRIMARY AUTHOR: Lya Vera

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

function OnCollisionEnter(other)
  -- Player takes coin
  if (other:HasTag("Player"))
  then

    print("Coin collision")

    local score = GameObject.FindByName("Stats")

    -- Increment score
    local script = score:GetScript("PlayerStats.lua")
    script.AddScore()

    -- Deactivate the object
    this:Deactivate()
  end
end -- fn end
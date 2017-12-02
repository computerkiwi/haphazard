--[[
FILE: CoinScript.lua
PRIMARY AUTHOR: Lya Vera

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

function OnCollisionEnter(other)
  
  -- Player takes coin
  if (other:HasTag("Player"))
  then

    local score = GameObject.FindByName("AllCoins")

    -- Increment score
    local script = score:GetScript("PlayerStats.lua")
    script.AddScore()

    -- TODO: Change to inactive?
    -- Delete the object
    --this:Delete()
  end

end -- fn end
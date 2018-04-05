--[[
FILE: CoinScript.lua
PRIMARY AUTHOR: Lya Vera

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

function Start()
  this:Activate()

  local tex = this:GetSprite().textureHandler
  tex.fps = 0
  tex.currentFrame = math.random(0, 4)
  this:GetSprite().textureHandler = tex
end -- fn end

function OnCollisionEnter(other)
  -- Player takes coin
  if (other:HasTag("Player"))
  then
    PlaySound("Coin6.wav", 1, 1, false)

    -- Deactivate the object
    this:Deactivate()
  end
end -- fn end
--[[
FILE: GnomeHealth.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

ENEMY_LAYER = 16;

health = 6
healthBar = nil

function Start()

  if(this:GetName() == "Player1")
  then
    healthBar = GameObject.FindByName("Player1Healthbar")
  elseif(this:GetName() == "Player2")
  then
    healthBar = GameObject.FindByName("Player2Healthbar")
  end

end


function OnCollisionEnter(other)

  if(this:GetDynamicCollider().colliderData:IsCollidingWithLayer(ENEMY_LAYER))
  then
    health = health - 1

	local th = healthBar:GetSprite().textureHandler
	th.currentFrame = health
	healthBar:GetSprite().textureHandler = th
  end

end

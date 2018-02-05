--[[
FILE: GnomeHealth.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]


ENEMY_LAYER = 16;

health = 6
healthBar = nil
invulTime = 0

INVULNERABLE_TIME = 2

LEVEL = "Level1.json"

function Start()

end

function Update(dt)
  
  if(invulTime > 0)
  then
    invulTime = invulTime - dt
  end
  
end

function OnCollisionEnter(other)

  invulTime = ScreenToWorld(vec2(-1,-1)).x
  if(invulTime <= 0 and this:GetDynamicCollider().colliderData:IsCollidingWithLayer(ENEMY_LAYER))
  then
    health = health - 1
	PlaySound("gnome_injure.mp3", 1.5, 1, false)

	invulTime = INVULNERABLE_TIME

	if(health <= 0)
	then
		-- Is dead
		this:GetScript("GnomeStatus.lua").isStatue = true
		this:GetScript("GnomeStatus.lua").statueHitPoints = this:GetScript("GnomeStatus.lua").STATUE_HIT_POINTS
	end
  end

end

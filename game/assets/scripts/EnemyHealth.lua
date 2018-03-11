--[[
FILE: EnemyBackAndForthMovement.lua
PRIMARY AUTHOR: Brett Schiff

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

-- Connections
otherPlayer = nil

ALLY_PROJECTILE_LAYER = 32 --1 << 5

-- Health stuff.
DAMAGE_FLASH_TIME = 0.1
damageFlashTimer = 0
health = 10

function UpdateDamageFlash(dt)
	damageFlashTimer = damageFlashTimer - dt

	local sprite = this:GetSprite()
	local color = sprite.color

	if (damageFlashTimer <= 0)
	then
		color.x = 1
		color.y = 1
		color.z = 1
		color.w = 1
	else
		color.x = 0.8
		color.y = 0
		color.z = 0
		color.w = 1
	end

	sprite.color = color
end

-- Updates each frame
function Update(dt)
	
	UpdateDamageFlash(dt)

end -- fn end

-- Other is a game object
function OnCollisionEnter(other)

  if(this:GetDynamicCollider().colliderData:IsCollidingWithLayer(ALLY_PROJECTILE_LAYER))
	then
		local projInfo = other:GetScript("ProjectileInfo.lua")

		health = health - projInfo.damage
		damageFlashTimer = DAMAGE_FLASH_TIME
	end
  if(health <= 0)
  then
    this:Destroy()
  end

end -- fn end

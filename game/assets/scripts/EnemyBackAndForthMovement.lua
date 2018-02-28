--[[
FILE: EnemyBackAndForthMovement.lua
PRIMARY AUTHOR: Brett Schiff

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

-- Connections
otherPlayer = nil

ALLY_PROJECTILE_LAYER = 32 --1 << 5
GROUND_LAYER = 1 << 3

-- Variables
direction = 1 -- direction of movement - -1 for left, 1 for right
speed = 2      -- speed at which boi moves
scaredSpeed = 3

-- Health stuff.
DAMAGE_FLASH_TIME = 0.1
damageFlashTimer = 0
health = 10


scared = false

-- Called at frame start
function Start()
	
end --fn end

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

	local velocity = this:GetRigidBody().velocity

	--[[local moveSpeed = speed

	if(scared == true)
	then

		moveSpeed = scaredSpeed

	end]]

	cast = Raycast.Cast(this:GetSpaceIndex(), this:GetTransform().position, vec2(0, -1), this:GetCollider().dimensions.y / 1.8, GROUND_LAYER)

	if(velocity.x == 0 or not cast.gameObjectHit:IsValid())
	then
		
		-- change direction
		direction = -direction

		-- Flip sprite
		if(direction == -1)
		then
			this:GetTransform().scale = vec3( -math.abs(this:GetTransform().scale.x), this:GetTransform().scale.y, 1 )
		elseif(direction == 1)
		then
			this:GetTransform().scale = vec3( math.abs(this:GetTransform().scale.x), this:GetTransform().scale.y, 1 )
		end

	end

	this:GetRigidBody().velocity = vec3(direction * speed, velocity.y, velocity.z)

end -- fn end

-- Other is a game object
function OnCollisionEnter(other)

  if(this:GetDynamicCollider().colliderData:IsCollidingWithLayer(ALLY_PROJECTILE_LAYER))
	then
		local projInfo = other:GetScript("ProjectileInfo.lua")

		scared = true
		health = health - projInfo.damage
		damageFlashTimer = DAMAGE_FLASH_TIME
	end
  if(health <= 0)
  then
    this:Destroy()
  end

end -- fn end

--[[
FILE: GnomeAbilities.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

--Prefabs Naming

--Prefab_Postfix_Standard = "Standard"
--Prefab_Postfix_Foot = "Foot"

-- Sprites

defaultGravity = 0

Sprite_RedGnome    = "redGnomeWalk.json"
Sprite_GreenGnome  = "greenGnomeWalk.json"
Sprite_BlueGnome   = "blueGnomeWalk.json"
Sprite_YellowGnome = "yellowGnomeWalk.json"
Sprite_RedGnome_Jump    = "Gnome_Red_Jump.png"
Sprite_GreenGnome_Jump  = "Gnome_Green_Jump.png"
Sprite_BlueGnome_Jump   = "Gnome_Blue_Jump.png"
Sprite_YellowGnome_Jump = "Gnome_Yellow_Jump.png"

local walkSprite
local jumpSprite

Blue_Foot_GravityScale = 0.5
Yellow_Foot_SpeedBoost = 2
Green_Foot_PushSpeed = 30

local usedFootAbilityThisJump = false

--[[

TODO::
Projectile Info
Gnome Abilities (this script)
Projectile Spawner features
GnomeSpawner finishing?

]]

function Start()
	SetType(this:GetScript("GnomeStatus.lua").GnomeType)

	defaultGravity = this:GetRigidBody().gravity
	--this:GetScript("GnomeMovement.lua").Knockback(vec2(1,1), 10)
end

function SetJumpSprite(jumping)
	-- Statue has its own sprite situation. Don't overwrite.
	if (this:GetScript("GnomeStatus.lua").isStatue)
	then
		return
	end
	
	if (jumping and this:GetScript("GnomeStatus.lua").stackedBelow == nil)
	then
		this:GetSprite().id = Resource.FilenameToID(jumpSprite)
	else
		this:GetSprite().id = Resource.FilenameToID(walkSprite)
	end
end

-- How long after a jump can we use a foot ability?
local JUMP_COOLDOWN = 0.1

function Update()
	this:GetScript("GnomeStatus.lua").specialMove = false
	this:GetRigidBody().gravity = defaultGravity
	this:GetScript("FollowingParticleSystem.lua").SetEnabled(false)

	local status = this:GetScript("GnomeStatus.lua")

  -- Attacks
  if(this:GetScript("InputHandler.lua").attackPressed and this:GetScript("GnomeHealth.lua").health > 0)
	then
		local type = status.GnomeType
		if(not status.stacked)
		then
			Attack(type)
		elseif(status.stackedBelow ~= nil)
    then
      local belowType = status.stackedBelow:GetScript("GnomeStatus.lua").GnomeType
      StackedAttack(type, belowType)
    end
	end
	-- Attacks

  -- Foot ability
  if(this:GetScript("GnomeMovement.lua").timeSinceJump >= JUMP_COOLDOWN and 
     status.stacked and status.stackedBelow == nil and 
     this:GetScript("GnomeHealth.lua").health > 0)
  then
    if (this:GetScript("InputHandler.lua").onJumpPress)
    then
      FootAbilityPress()
    end
    if (this:GetScript("InputHandler.lua").jumpPressed)
    then
      FootAbilityHold()
    end
  end
end

function Jump()
	usedFootAbilityThisJump = false
end

local yellowJumps = 2

function FootAbilityHold()  
  
	local type = this:GetScript("GnomeStatus.lua").GnomeType
  
  if(type == 3 and usedFootAbilityThisJump) -- Blue
	then
		-- Lowers fall speed
		this:GetRigidBody().gravity = vec3(defaultGravity.x * Blue_Foot_GravityScale, defaultGravity.y * Blue_Foot_GravityScale,0)
		this:GetScript("FollowingParticleSystem.lua").SetEnabled(true)
	end

end
function FootAbilityPress()  
  
	local type = this:GetScript("GnomeStatus.lua").GnomeType

	-- Set sprite
	if(type == 1)	-- Red
	then
		if(usedFootAbilityThisJump == false)
		then
			this:GetScript("GnomeMovement.lua").Jump()
			this:GetScript("ProjectileSpawner.lua").Fire("Projectile_Red_Foot.json")
			this:GetScript("FollowingParticleSystem.lua").SetEnabled(true)
		end
	elseif(type == 2)	-- Green
	then
		if(usedFootAbilityThisJump == false)
		then
		local moveScript = this:GetScript("GnomeMovement.lua")
		
  		moveScript.Knockback(vec2(moveScript.facing, 0.4), Green_Foot_PushSpeed)
		this:GetScript("ProjectileSpawner.lua").Fire("Projectile_Green_Foot.json")
		end
	elseif(type == 4)	-- Yellow
	then
    if (not usedFootAbilityThisJump)
    then
      yellowJumps = 2
    end
  
		if(yellowJumps > 0)
		then
      local moveScript = this:GetScript("GnomeMovement.lua")
      moveScript.Jump(0.75)
      -- TODO: Spawn ability particles here.

      yellowJumps = yellowJumps - 1
		end
	end
  
	usedFootAbilityThisJump = true
end

function Attack(type)
	PrefabName = "Projectile_" .. TypeName(type) .."_Standard.json"
	this:GetScript("ProjectileSpawner.lua").Fire(PrefabName)
	--this:GetScript("ProjectileSpawner.lua").Fire("waterProjectile.json")

end

function StackedAttack(type, belowType)

	PrefabName = "Projectile_" .. TypeName(type) .."_Stacked.json"
	this:GetScript("ProjectileSpawner.lua").Fire(PrefabName)
	--this:GetScript("ProjectileSpawner.lua").Fire("otherProjectile.json")

end

function SetType(type)
	this:GetScript("GnomeStatus.lua").GnomeType = type

	-- Set sprite
	if(type == 1)	-- Red
	then
		walkSprite = Sprite_RedGnome
		jumpSprite = Sprite_RedGnome_Jump
		this:GetScript("FollowingParticleSystem.lua").ParticlePrefab = "assets/prefabs/Particles_Red_Jump.json"
		this:GetScript("FollowingParticleSystem.lua").InitParticles()
	elseif(type == 2)	-- Green
	then
		walkSprite = Sprite_GreenGnome
		jumpSprite = Sprite_GreenGnome_Jump
	elseif(type == 3)	-- Blue
	then
		walkSprite = Sprite_BlueGnome
		jumpSprite = Sprite_BlueGnome_Jump
		this:GetScript("FollowingParticleSystem.lua").ParticlePrefab = "assets/prefabs/Particles_Blue_Jump.json"
		this:GetScript("FollowingParticleSystem.lua").PARTICLE_OFFSET_Y = -0.5
		this:GetScript("FollowingParticleSystem.lua").InitParticles()
	elseif(type == 4)	-- Yellow
	then
		walkSprite = Sprite_YellowGnome
		jumpSprite = Sprite_YellowGnome_Jump
	end
	
	SetJumpSprite(false)
end

function TypeName(type)
	if(type == 1)
	then
		return "Red"
	elseif(type == 2)
	then
		return "Green"
	elseif(type == 3)
	then
		return "Blue"
	elseif(type == 4)
	then
		return "Yellow"
	end
end

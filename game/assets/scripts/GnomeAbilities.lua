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
	
	if (jumping and not this:GetScript("GnomeStatus.lua").stacked)
	then
		this:GetSprite().id = Resource.FilenameToID(jumpSprite)
	else
		this:GetSprite().id = Resource.FilenameToID(walkSprite)
	end
end

function Update()
	this:GetScript("GnomeStatus.lua").specialMove = false
	this:GetRigidBody().gravity = defaultGravity
	this:GetScript("FollowingParticleSystem.lua").SetEnabled(false)

	local status = this:GetScript("GnomeStatus.lua")

	-- Attacks
	if(this:GetScript("InputHandler.lua").attackPressed and this:GetScript("GnomeHealth.lua").health > 0)
	then
		local type = status.GnomeType
		if(status.stacked)
		then
			if(status.stackedBelow == nil)
			then
				FootAbility()
			else
				local belowType = status.stackedBelow:GetScript("GnomeStatus.lua").GnomeType
				StackedAttack(type, belowType)
			end
		else
			Attack(type)
		end
	end

end

function Jump()
	usedFootAbilityThisJump = false
end

function FootAbility()
	
	local type = this:GetScript("GnomeStatus.lua").GnomeType

	-- Set sprite
	if(type == 1)	-- Red
	then
		if(usedFootAbilityThisJump == false)
		then
			this:GetScript("GnomeMovement.lua").Jump()
			this:GetScript("ProjectileSpawner.lua").Fire("Projectile_Red_Foot.json")
			usedFootAbilityThisJump = true
			this:GetScript("FollowingParticleSystem.lua").SetEnabled(true)
		end
	elseif(type == 2)	-- Green
	then
		if(usedFootAbilityThisJump == false)
		then
		local moveScript = this:GetScript("GnomeMovement.lua")
		
  		moveScript.Knockback(vec2(moveScript.facing, 0.4), Green_Foot_PushSpeed)
		this:GetScript("ProjectileSpawner.lua").Fire("Projectile_Green_Foot.json")

		usedFootAbilityThisJump = true
		end
	elseif(type == 3)	-- Blue
	then
		-- Lowers fall speed
		this:GetRigidBody().gravity = vec3(defaultGravity.x * Blue_Foot_GravityScale, defaultGravity.y * Blue_Foot_GravityScale,0)
		this:GetScript("FollowingParticleSystem.lua").SetEnabled(true)
	elseif(type == 4)	-- Yellow
	then
		this:GetScript("GnomeStatus.lua").specialMove = true
		this:GetScript("GnomeStatus.lua").specialMoveScale = Yellow_Foot_SpeedBoost
	end

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

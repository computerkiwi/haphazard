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

function Update()
	this:GetScript("GnomeStatus.lua").specialMove = false
	this:GetRigidBody().gravity = defaultGravity

	local status = this:GetScript("GnomeStatus.lua")

	-- Attacks
	if(this:GetScript("InputHandler.lua").attackPressed)
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

		print(this:GetRigidBody().gravity .y)
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

	PrefabName = "Projectile_" .. TypeName(type) .."_on" .. TypeName(belowType) .. ".json"
	this:GetScript("ProjectileSpawner.lua").Fire(PrefabName)
	--this:GetScript("ProjectileSpawner.lua").Fire("otherProjectile.json")

end

function SetType(type)
	this:GetScript("GnomeStatus.lua").GnomeType = type

	-- Set sprite
	if(type == 1)	-- Red
	then
		this:GetSprite().id = Resource.FilenameToID(Sprite_RedGnome)
	elseif(type == 2)	-- Green
	then
		this:GetSprite().id = Resource.FilenameToID(Sprite_GreenGnome)
	elseif(type == 3)	-- Blue
	then
		this:GetSprite().id = Resource.FilenameToID(Sprite_BlueGnome)
	elseif(type == 4)	-- Yellow
	then
		this:GetSprite().id = Resource.FilenameToID(Sprite_YellowGnome)
	end
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

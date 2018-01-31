--[[
FILE: GnomeAbilities.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

--Prefabs Naming

--Prefab_Postfix_Standard = "Standard"
--Prefab_Postfix_Foot = "Foot"

-- Sprites

local defaultGravity

Sprite_RedGnome    = "redGnomeWalk.json"
Sprite_GreenGnome  = "greenGnomeWalk.json"
Sprite_BlueGnome   = "blueGnomeWalk.json"
Sprite_YellowGnome = "yellowGnomeWalk.json"

Blue_Foot_GravityScale = 0.5
Yellow_Foot_SpeedBoost = 2
Green_Foot_PushSpeed = 10

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
	local status = this:GetScript("GnomeStatus.lua")

	-- Attacks
	if(this:GetScript("InputHandler.lua").attackPressed)
	then
		if(status.stacked)
		then
			if(status.stackedBelow == nil)
			then
				FootAbility()
			else
				local type = status.GnomeType
				local belowType = status.stackedBelow:GetScript("GnomeStatus.lua").type

				StackedAttack(type, belowType)
			end
		else
			Attack()
		end
	end

end

function LateUpdate()
	-- Resets abilities that happen once per update (movement boost / gravity changes)

	this:GetScript("GnomeStatus.lua").specialMove = false
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
			usedFootAbilityThisJump = true
		end
	elseif(type == 2)	-- Green
	then
		if(usedFootAbilityThisJump == false)
		then
		this:GetScript("GnomeMovement.lua").Knockback(vec2(1,0), Green_Foot_PushSpeed)
		--[[
			local v = this:GetRigidBody().velocity
			this:GetRigidBody().velocity = vec3(v.x + Green_Foot_PushSpeed, v.y, 0)
			usedFootAbilityThisJump = true]]
		end
	elseif(type == 3)	-- Blue
	then
		-- Lowers fall speed
		this:GetRigidBody().gravity = defaultGravity * Blue_Foot_GravityScale
	elseif(type == 4)	-- Yellow
	then
		this:GetScript("GnomeStatus.lua").specialMove = true
		this:GetScript("GnomeStatus.lua").specialMoveScale = Yellow_Foot_SpeedBoost
	end

end

function Attack()

	--PrefabName = "Projectile_" .. TypeName(type) .."_Standard.json"
	--this:GetScript("ProjectileSpawner.lua").Fire(PrefabName)
	this:GetScript("ProjectileSpawner.lua").Fire("waterProjectile.json")

end

function StackedAttack(type, belowType)

	--PrefabName = "Projectile_" .. TypeName(type) .."_on" TypeName(belowType)
	--this:GetScript("ProjectileSpawner.lua").Fire(PrefabName)
	this:GetScript("ProjectileSpawner.lua").Fire("otherProjectile.json")

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

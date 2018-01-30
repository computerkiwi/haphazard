--[[
FILE: GnomeAbilities.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

--Prefabs Naming

--Prefab_Postfix_Standard = "Standard"
--Prefab_Postfix_Foot = "Foot"

-- Sprites

Sprite_RedGnome    = "redGnomeWalk.json"
Sprite_GreenGnome  = "greenGnomeWalk.json"
Sprite_BlueGnome   = "blueGnomeWalk.json"
Sprite_YellowGnome = "yellowGnomeWalk.json"


--[[

TODO::
Projectile Info
Gnome Abilities (this script)
Projectile Spawner features
GnomeSpawner finishing?

]]

function Start()
	SetType(this:GetScript("GnomeStatus.lua").GnomeType)

	this:GetScript("GnomeMovement.lua").Knockback(vec2(1,1), 10)
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
end

function FootAbility()
	
	this:GetScript("GnomeStatus.lua").GnomeType = type

	-- Set sprite
	if(type == 1)	-- Red
	then
		this:GetScript("GnomeMovement.lua").Jump()
	elseif(type == 2)	-- Green
	then
		--Todo: make only able to use once per jump

	elseif(type == 3)	-- Blue
	then
	elseif(type == 4)	-- Yellow
	then
		
	end

end

function Stack()

end

function Unstack()

end

function Attack()

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

function StackedAttack(type, belowType)
	
	

end
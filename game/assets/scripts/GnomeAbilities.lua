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
GnomeSpawner sprite

]]

function Start()
	SetType(3)
end

function Jump()
	
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

--[[
FILE: GnomeSpawner.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

Player1Prefab = "Player1Gnome.json"
Player2Prefab = "Player2Gnome.json"
Player3Prefab = "Player3Gnome.json"
Player4Prefab = "Player4Gnome.json"

GnomeObject = nil

-- Player Settings --

SpawnMe = false
GnomeType = 1  

--[[
Types:
Red	   = 1
Green  = 2
Blue   = 3
Yellow = 4
]]

function Start()
	if(SpawnMe)
	then
		Join()
	end
end

function Update()
	if(SpawnMe and GnomeObject == nil)
	then
		Join()
	end
end

function Join()
	SpawnMe = true

	if(GnomeType == 1)
	then
		GnomeObject = GameObject.LoadPrefab(Player1Prefab)
	elseif(GnomeType == 2)
	then
		GnomeObject = GameObject.LoadPrefab(Player2Prefab)
	elseif(GnomeType == 3)
	then
		GnomeObject = GameObject.LoadPrefab(Player3Prefab)
	elseif(GnomeType == 4)
	then
		GnomeObject = GameObject.LoadPrefab(Player4Prefab)
	end

end

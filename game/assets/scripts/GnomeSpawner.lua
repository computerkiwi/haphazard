--[[
FILE: GnomeSpawner.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

GnomePrefab = "GnomePlayer1.json"

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

function Join()
	SpawnMe = true

	GnomeObject = GameObject.LoadPrefab(GnomePrefab)

	if(GnomeType == 1)
	then
		
	end

end

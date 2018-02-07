--[[
FILE: GnomeSpawner.lua
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

SPAWNER_WIDTH = 5

function Start()
	local gnomes = PLAYER_MANAGER.SpawnAllPlayers()
	local gnomeCount = #gnomes
	
	local pos = this:GetTransform().position
	local leftX = pos.x - (SPAWNER_WIDTH / 2)
	
	for i,gnome in ipairs(gnomes)
	do
		pos.x = leftX + (i - 1) * (SPAWNER_WIDTH / gnomeCount)
		gnome:GetTransform().position = pos
	end
end

function Update(dt)
end

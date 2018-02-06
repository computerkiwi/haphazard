--[[
FILE: GnomeSafety.lua
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

-- Script to delete gnomes that were accidentally saved.
-- Gnomes should be spawned by spawners instead of saved.

function Start()
	local status = this:GetScript("GnomeStatus.lua")
	local gnomeObj = PLAYER_MANAGER.GetByPlayerID(status.PLAYER_NUM).gameObject
	
	print("This: " .. tostring(this.id))
	print("Gnome: " .. tostring(gnomeObj.id))
	
	if (gnomeObj.id ~= this.id)
	then
		this:Destroy()
	end
end

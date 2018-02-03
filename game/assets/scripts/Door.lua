--[[
FILE: Door.lua
PRIMARY AUTHOR: Ash Duong

Copyright (c) 2017 DigiPen (USA) Corporation.
]]--

DESTINATION_LEVEL		= "MainMenu.json"	
GNOME_HERE 				= false
CURRENT_GNOME 			= nil

function Update(dt)
	if((GNOME_HERE == true) and (CURRENT_GNOME ~= nil))
	then
		if(CURRENT_GNOME:GetScript("InputHandler.lua").jumpPressed)
		then
			Engine.LoadLevel(DESTINATION_LEVEL)
		end
	end

end

function OnCollisionEnter(other)
	if(other:HasTag("Player"))
	then
	GNOME_HERE = true
	CURRENT_GNOME = other
	end
end

function OnCollisionExit(other)

	GNOME_HERE = false
end
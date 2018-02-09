--[[
FILE: Door.lua
PRIMARY AUTHOR: Ash Duong

Copyright (c) 2017 DigiPen (USA) Corporation.
]]--

DESTINATION_LEVEL		= "MainMenu.json"	 

function OnCollisionEnter(other)
	if(other:HasTag("Player"))
	then
		if(other:GetScript("InputHandler.lua").jumpPressed)
		then
			Engine.LoadLevel(DESTINATION_LEVEL)
		end
	end
end

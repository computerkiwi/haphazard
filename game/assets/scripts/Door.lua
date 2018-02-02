--[[
FILE: Door.lua
PRIMARY AUTHOR: Ash Duong

Copyright (c) 2017 DigiPen (USA) Corporation.
]]--

MAINMENU		= "MainMenu.json"
LEVELSELECT		= "LevelSelect.json"
ACT1_LEVEL1		= "1_1.json"
ACT1_LEVEL2		= "1_2.json"	
GNOME_HERE 		= false
CURRENT_GNOME 	= nil

function Update(dt)
	if((GNOME_HERE == true) and (CURRENT_GNOME ~= nil))
	then
		if(CURRENT_GNOME:GetScript("InputHandler.lua").jumpPressed)
		then
			Door(this:GetName())
		end
	end

end

function OnCollisionEnter(other)

	GNOME_HERE = true
	CURRENT_GNOME = other

end

function OnCollisionExit(other)

	GNOME_HERE = false

end

function Door(name)
	if		(name == "MainMenuDoor")
		then Engine.LoadLevel(MAINMENU)
	elseif	(name == "LevelSelectDoor")
		then Engine.LoadLevel(LEVELSELECT) 
	elseif	(name == "1_1Door")
		then Engine.LoadLevel(ACT1_LEVEL1)
	elseif	(name == "1_2Door")
		then Engine.LoadLevel(ACT1_LEVEL2)
	end
end
--[[
FILE: InteractiveMachine.lua
PRIMARY AUTHOR: Ash Duong

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

Activated = false
GEM_SPRITE = "Interactive(Green)_On.png"

function OnCollisionEnter(other)
	if (other:HasTag("Player") and other:GetScript("GnomeStatus.lua").hasGem == true)
	then
		Activated = true
    this:GetSprite().id = Resource.FilenameToID(GEM_SPRITE)

    -- Reset gnome to not have gem
    other:GetScript("GnomeStatus.lua").hasGem = false
	end
end
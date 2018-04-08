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
		PlaySound("Rocks1.wav", 1, 1, false)
			Activated = true
		this:GetSprite().id = Resource.FilenameToID(GEM_SPRITE)

		-- Reset gnome to not have gem
		other:GetScript("GnomeStatus.lua").hasGem = false
		
		-- We're no longer important to focus the camera onto once activated.
		local focusScript = this:GetScript("CameraFocus.lua")
		if (focusScript)
		then
			focusScript.enabled = false
		end
	end
end
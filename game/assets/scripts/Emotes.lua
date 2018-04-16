--[[
FILE: Emotes.lua
PRIMARY AUTHOR: Ash Duong

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

--Variables
Player = nil

function Update(dt)
	if (Player:IsValid())
	then
		local PlayerStatus = Player:GetScript("GnomeStatus.lua")
		local sprite = this:GetSprite()
		local th = sprite.textureHandler

		if (PlayerStatus.hasGem == true)
		then
			th.currentFrame = 1
		else
			th.currentFrame = 0
		end

		if (PlayerStatus.overDoor == true)
		then
			th.currentFrame = 2
		else
			th.currentFrame = 0
		end

		sprite.textureHandler = th
	end
end
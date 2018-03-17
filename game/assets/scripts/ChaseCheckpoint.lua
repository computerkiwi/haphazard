--[[
FILE: ChaseCheckpoint.lua
PRIMARY AUTHOR: Max

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

unlocked = false

function Start()
	unlocked = false
end

function OnCollisionEnter(other)
	if (other:HasTag("Player") and not unlocked)
	then
		unlocked = true
		local PM = _G.PLAYER_MANAGER
		for k, player in pairs(PM.PLAYERS)
		do
			local p = player.gameObject
			local status = p:GetScript("GnomeStatus.lua")

			if(status.killedByChaseBox)
			then
				status.killedByChaseBox = false
				p:GetTransform().position = this:GetTransform().position
				p:GetScript("GnomeHealth.lua").statueHitPoints = 0
				p:GetScript("GnomeHealth.lua").ChipStatue()
			end

		end
	end
end

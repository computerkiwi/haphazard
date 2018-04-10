--[[
FILE: GrowAndKillYourself.lua
PRIMARY AUTHOR: Brett Schiff

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

local TIME_TO_DELETE = 1
local growthMultiplier = .1

timer = 0

function Update(dt)

	timer = timer + dt
	
	if (timer > TIME_TO_DELETE)
	then
		this:Destroy()
		return
	end

	local transform = this:GetTransform()

	local newScale = transform.scale

	newScale.x = newScale.x + newScale.x * growthMultiplier * dt
	newScale.y = newScale.y + newScale.y * growthMultiplier * dt

	transform.scale = newScale

end
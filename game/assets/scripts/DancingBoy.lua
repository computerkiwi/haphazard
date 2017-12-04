--[[
FILE: DancingBoy.lua
PRIMARY AUTHOR: Max

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

DANCE_TIME = 0.35
time = 0.35


function Update(dt)
	time = time - dt
	
	this:GetTransform().scale = vec3(this:GetTransform().scale.x, 1.25 + 0.25 * math.cos(6.28 / DANCE_TIME * time), 1)

	this:GetTransform().position = vec2(this:GetTransform().position.x, 0.13 + 0.25/2 * math.cos(6.28 / DANCE_TIME *time))

	if(time < 0)
	then
		this:GetTransform().scale = vec3(-this:GetTransform().scale.x, this:GetTransform().scale.y, 1)
		time = DANCE_TIME
	end
end

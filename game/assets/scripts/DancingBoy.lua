--[[
FILE: DancingBoy.lua
PRIMARY AUTHOR: Max

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

DANCE_TIME = 0.35
time = 0.35

startYScale = 0
standingY = 0
startY = 0
delta = 0

inAir = true

function Start()

	startYScale = this:GetTransform().scale.y

	--DANCE_TIME = math.random() + 0.1
	time = DANCE_TIME
	standingY = this:GetTransform().position.y

end


function Update(dt)
	if(dt == 0) then return	end
	
	time = time - dt
	local delta = 0.25 * math.cos(6.28 / DANCE_TIME *(time));

	this:GetTransform().scale = vec3(this:GetTransform().scale.x, startYScale + delta, 1)

	if(this:GetRigidBody().velocity.y != 0) 
	then 
		inAir = true 
	end

	if(inAir)
	then
		if(this:GetRigidBody().velocity.y == 0)
		then
			inAir = false;
			standingY = this:GetTransform().position.y
		end
	else
		this:GetTransform().position = vec2(this:GetTransform().position.x, standingY + delta)
	end


	if(time < -DANCE_TIME)
	then
		this:GetTransform().scale = vec3(-this:GetTransform().scale.x, this:GetTransform().scale.y, 1)
		time = DANCE_TIME
	end
end

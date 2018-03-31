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

inAir = false
flip = true
oscillationIntensity = .25

function Start()

	startYScale = this:GetTransform().scale.y

	--DANCE_TIME = math.random() + 0.1
	time = DANCE_TIME
	standingY = this:GetTransform().position.y

end


function Update(dt)
	if(dt == 0) then return	end
	
	if(this:GetRigidBody() ~= nil and math.abs(this:GetRigidBody().velocity.y) > 1 ) 
	then
		inAir = false
	end


	if(inAir)
	then
		if(this:GetRigidBody() ~= nil and math.abs(this:GetRigidBody().velocity.y) <= 0.1)
		then
			inAir = false;
			standingY = this:GetTransform().position.y
		end
	else
		time = time - dt
		local delta = oscillationIntensity * math.cos(6.28 / DANCE_TIME *(time));

		this:GetTransform().scale = vec3(this:GetTransform().scale.x, startYScale + delta, 1)

		if(this:GetRigidBody() == nil)
		then
			this:GetTransform().position = vec2(this:GetTransform().position.x, standingY + delta/2)
		end
		
		if(time < 0 and flip)
		then
			if(this:GetRigidBody() == nil or math.abs(this:GetRigidBody().velocity.x) < 0.1)
			then
				this:GetTransform().scale = vec3(-this:GetTransform().scale.x, this:GetTransform().scale.y, 1)
			end
			time = DANCE_TIME
		end
	end

end

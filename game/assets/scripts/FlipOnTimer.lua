--[[
FILE: FlipOnTimer.lua
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

TIME = 1
timer = TIME

function Update(dt)
	timer = timer - dt
	if (timer < 0)
	then
		timer = timer + TIME
		
		local rBody = this:GetRigidBody()
		local temp = rBody.velocity
		temp.x = temp.x * -1
		rBody.velocity = temp
	end
	
	
end

--[[
FILE: HalfwayFloat.lua
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2017 DigiPen (USA) Corporation.
]]
TARGET_NAME = "Ball"

function Update(dt)
	print(0)
	if (IsPressed(0)) --Mouse_1
	then
	print(1)
		local thisPos = this:GetTransform().position
	
	print(2)
		local target = GameObject.FindByName(TARGET_NAME)
		local transform = target:GetTransform()
		
		local diff = vec2.Sub(GetMousePos(), thisPos)
		diff.x = diff.x * 0.5
		diff.y = diff.y * 0.5
		
		transform.position = vec2.Add(thisPos, diff)
		
	end
end
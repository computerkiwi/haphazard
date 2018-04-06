--[[
FILE: CreditScroller.lua
PRIMARY AUTHOR: Brett Schiff

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

local scrollSpeed = 5

local transform = nil


function Start()

	transform = this:GetTransform()

end

function Update(dt)

	transform.position = vec2(transform.position.x, transform.position.y + scrollSpeed * dt)

end

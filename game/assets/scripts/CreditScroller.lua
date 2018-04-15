--[[
FILE: CreditScroller.lua
PRIMARY AUTHOR: Brett Schiff

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

local scrollSpeed = 1.5

local transform = nil

function ContinuePressed()
	-- Spacebar
	if (IsHeld(KEY.Space))
	then
		return true
	end
	
	-- A Button on the gamepads
	for player = 0,3
	do
		local A_BUTTON = 0
		if (GamepadIsHeld(player, A_BUTTON))
		then
			return true
		end
	end
	
	-- Failed to find a continue.
	return false
end

function Start()

	transform = this:GetTransform()

end

function Update(dt)
	
	if(ContinuePressed())
	then

		transform.position = vec2(transform.position.x, transform.position.y + scrollSpeed * dt * _G.spaceScrollMultiplier)

	else

		transform.position = vec2(transform.position.x, transform.position.y + scrollSpeed * dt)

	end



end

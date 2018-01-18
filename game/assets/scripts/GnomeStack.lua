--[[
FILE: GnomeStack.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

gnomeStackDistance = 0.5

-- Other is a game object
function OnCollisionEnter(other)

	-- Player collides with other player
	if (other:HasTag("Player") and this:GetScript("GnomeStatus.lua").stacked == false)
	then
		StackPlayers(other)
	end

end

function StackPlayers(other)

	-- Get transforms
	local playerTransform = this:GetTransform()
	local otherTransform = other:GetTransform()

	-- Get positions
	local playerPos = playerTransform.position
	local otherPos = otherTransform.position

	if(otherPos.y > playerPos.y)
	then
		return -- Let top handle stacking for both
	end
	
	local newPos = playerPos
	newPos.x = 0;
	newPos.y = newPos.y + gnomeStackDistance

	otherTransform.position = newPos

	otherTransform.parent = this

	SetLayersNotColliding(PLAYER_LAYER, PLAYER_LAYER)

	this:GetScript("GnomeStatus.lua").stacked = true;
	other:GetScript("GnomeStatus.lua").stacked = true;

end -- fn end

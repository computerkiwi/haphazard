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

	-- Get status
	local thisStatus = this:GetScript("GnomeStatus.lua")
	local otherStatus = other:GetScript("GnomeStatus.lua")

	if(otherPos.y > playerPos.y)
	then
		return -- Let bottom handle stacking for both
	end
	
	local newPos = playerPos
	newPos.y = newPos.y + gnomeStackDistance

	otherTransform.position = newPos

	otherTransform.parent = this

	--SetLayersNotColliding(thisStatus.PLAYER_PHYS_LAYER , otherStatus.PLAYER_PHYS_LAYER)

	thisStatus.stacked = true;
	otherStatus.stacked = true;

end -- fn end

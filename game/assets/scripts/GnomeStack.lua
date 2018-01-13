--[[
FILE: GnomeStack.lua
PRIMARY AUTHOR: Ash Duong

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

-- Gnome type (number?)
gnomeType	= 0 -- 0 Red, 1 Blue, 2 Yellow, 3 Green
stackedOn	= nil -- Which gnomeType is this stacked on?
isFoot		= false -- Is this Gnome the bottom of the stack?

-- Other is a game object
function OnCollisionEnter(other)
	-- Player collides with other player
	if (other:HasTag("Player") and stackEnabled == false)
	then
		StackPlayers(other)
	end
end -- fn end

-- Kieran's stack code
function StackPlayers(other)
	-- Get transforms
	local playerTransform = this:GetTransform()
	local otherTransform = other:GetTransform()
		
	-- Get positions
	local playerPos = playerTransform.position
	local otherPos = otherTransform.position

	local snapDistance = 1 -- horizontal distance from other gnome
	local xDistance = playerPos.x -- x-axis distance between players

	-- How do I get the absolute value? Hmmmm
	if (playerPos.x > otherPos.x)
	then
		xDistance = playerPos.x - otherPos.x
	else
		xDistance = otherPos.x - playerPos.x
	end

	-- TODO: Change to use raycast downwards to check for gnome collision
	if (playerPos.y > otherPos.y)
	then
		-- Players are stacked
		stackEnabled = true
		PlaySound("stack_off.mp3", 0.5, 1, false)

		-- Set bottom player as parent
		stackParent = other

		-- Don't detect collision between these layers
		SetLayersNotColliding(PLAYER_LAYER, PLAYER_LAYER)

		-- TEMP SOLUTION: make sure gnome is drawn in front
		--if (playerTransform.zLayer < otherTransform.zLayer)
		--then
		--	playerTransform.zLayer = otherTransform.zLayer - 1
		--end
	end
end -- fn end
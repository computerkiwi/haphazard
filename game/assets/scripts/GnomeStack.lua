--[[
FILE: GnomeStack.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

gnomeStackDistance = 0.6

function Update(dt)
	
	-- Get status
	local thisStatus = this:GetScript("GnomeStatus.lua")

	if(thisStatus.stacked and thisStatus.stackedBelow ~= nil) -- Is on top
	then
		local parent = thisStatus.stackedBelow

		local parentStatus = parent:GetScript("GnomeStatus.lua")

		-- Get transforms
		local playerTransform = this:GetTransform()
		local parentTransform = parent:GetTransform()

		-- Get positions
		local playerPos = playerTransform.position
		local parentPos = parentTransform.position
	
		local newPos = parentPos
		newPos.y = newPos.y + gnomeStackDistance
		
		playerTransform.position = newPos

		this:GetRigidBody().velocity = vec3(0,0,0)
	end

end

function StackPlayers(other)

	-- Get status
	local thisStatus = this:GetScript("GnomeStatus.lua")
	local otherStatus = other:GetScript("GnomeStatus.lua")

	-- Get positions
	local thisPos = this:GetTransform().position
	local otherPos = other:GetTransform().position
	
	if(thisPos.y < otherPos.y) --Bottom gnome handles stacking
	then
		SetLayersNotColliding(thisStatus.PLAYER_PHYS_LAYER, otherStatus.PLAYER_PHYS_LAYER)

		thisStatus.stacked = true
		otherStatus.stacked = true

		-- This = bottom, other = top
		thisStatus.stackedAbove = other
		otherStatus.stackedBelow = this
	end

end -- fn end

-- Other is a game object
function OnCollisionEnter(other)
	-- Player collides with other player
	if (other:HasTag("Player") and this:GetScript("GnomeStatus.lua").stackedAbove == nil)
	then
		StackPlayers(other)
	end
end


function Disconnect()
	local thisStatus = this:GetScript("GnomeStatus.lua")
	local parentStatus = thisStatus.stackedBelow:GetScript("GnomeStatus.lua")

	SetLayersColliding(thisStatus.PLAYER_PHYS_LAYER , parentStatus.PLAYER_PHYS_LAYER)

	-- Cut connection to gnome below 
	if(parentStatus.stackedBelow == nil) -- If gnome is not stacked on another (is bottom of stack)
	then
		parentStatus.stacked = false
	end

	parentStatus.stackedAbove = nil
	thisStatus.stackedBelow = nil
end


function Unstack()
	-- Get status
	local thisStatus = this:GetScript("GnomeStatus.lua")

	if(thisStatus.stacked)
	then
		if(thisStatus.stackedBelow ~= nil) -- If is middle of stack. Cut connection to bottom
		then
			Disconnect()
		end

		if(stackedAbove == nil) -- If is top
		then
			-- Not connected to anything anymore
			thisStatus.stacked = false
		end
	end

end

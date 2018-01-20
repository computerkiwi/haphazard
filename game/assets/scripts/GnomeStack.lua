--[[
FILE: GnomeStack.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

gnomeStackDistance = 0.5

function Update(dt)
	
	-- Get status
	local thisStatus = this:GetScript("GnomeStatus.lua")

	if(thisStatus.stacked and thisStatus.stackedParent ~= nil) -- Is on top
	then
		local parent = thisStatus.stackedParent

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

-- Other is a game object
function OnCollisionEnter(other)

	-- Player collides with other player
	if (other:HasTag("Player") and this:GetScript("GnomeStatus.lua").stacked == false and other:GetScript("GnomeStatus.lua").isParent == false)
	then
		StackPlayers(other)
	end

end

function StackPlayers(other)

	-- Get status
	local thisStatus = this:GetScript("GnomeStatus.lua")
	local otherStatus = other:GetScript("GnomeStatus.lua")

	-- Get transforms
	local playerTransform = this:GetTransform()
	local otherTransform = other:GetTransform()

	-- Get positions
	local playerPos = playerTransform.position
	local otherPos = otherTransform.position
	
	if(playerPos.y > otherPos.y)
	then
		return -- Let bottom (this) handle stacking for both
	end
	
	SetLayersNotColliding(thisStatus.PLAYER_PHYS_LAYER, otherStatus.PLAYER_PHYS_LAYER)

	if(otherStatus.isParent)
	then
		SetLayersNotColliding(thisStatus.PLAYER_PHYS_LAYER, otherStatus.PLAYER_PHYS_LAYER)
	end

	thisStatus.stacked = true
	otherStatus.stacked = true
	otherStatus.isParent = true

	otherStatus.stackedParent = this;

end -- fn end


function Unstack()
	-- Get status
	local thisStatus = this:GetScript("GnomeStatus.lua")

	if(thisStatus.stacked)
	then
		if(isParent) -- Is not top (bottom or middle)
		then
			if(stackedParent ~= nil) -- Is middle, cut connection to bottom
			then
				SetLayersColliding(thisStatus.PLAYER_PHYS_LAYER , thisStatus.stackedParent:GetScript("GnomeStatus.lua").PLAYER_PHYS_LAYER)

				-- Cut connection to gnome below 
				thisStatus.stackedParent:GetScript("GnomeStatus.lua").isParent = false
				thisStatus.stackedParent = nil 
			end
			-- Else is bottom, do nothing, can't unstack from below
		else
			thisStatus.stacked = false     -- Not connected to anything anymore
			
			SetLayersColliding(thisStatus.PLAYER_PHYS_LAYER , thisStatus.stackedParent:GetScript("GnomeStatus.lua").PLAYER_PHYS_LAYER)

			-- Cut connection to gnome below 
			thisStatus.stackedParent:GetScript("GnomeStatus.lua").isParent = false
			thisStatus.stackedParent = nil

		end
	end

end

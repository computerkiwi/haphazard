--[[
FILE: GnomeStack.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

gnomeStackDistance = 0.6

currCollisionLayer = 0
delayCollisionCounter = 0
delayCollisionLayer = 0

function Start()
	
	currCollisionLayer = CollisionLayer(this:GetScript("GnomeStatus.lua").PLAYER_PHYS_LAYER)

end

function Update(dt)

	if(this:GetScript("GnomeStatus.lua").stackedBelow ~= nil)
	then
		currCollisionLayer = this:GetScript("GnomeStatus.lua").stackedBelow:GetCollider().collisionLayer
		delayCollisionCounter = 0
		delayCollisionLayer = 0
	elseif(delayCollisionLayer == 0)
	then
		currCollisionLayer = CollisionLayer(this:GetScript("GnomeStatus.lua").PLAYER_PHYS_LAYER)
	end
	this:GetCollider().collisionLayer = currCollisionLayer

	if(delayCollisionLayer ~= 0)
	then
		delayCollisionCounter = delayCollisionCounter - dt
		
		if(delayCollisionCounter < 0)
		then
			currCollisionLayer = CollisionLayer(delayCollisionLayer)
			this:GetCollider().collisionLayer = currCollisionLayer

			-- Avoid 1 update "snap" if a gnome is on top
			local above = this:GetScript("GnomeStatus.lua").stackedAbove
			if(above ~= nil)
			then
				above:GetScript("GnomeStack.lua").currCollisionLayer = currCollisionLayer
				above:GetCollider().collisionLayer = currCollisionLayer

				local aboveAbove = above:GetScript("GnomeStatus.lua").stackedAbove
				if(aboveAbove ~= nil)
				then
					aboveAbove:GetScript("GnomeStack.lua").currCollisionLayer = currCollisionLayer
					aboveAbove:GetCollider().collisionLayer = currCollisionLayer
				end
			end

			delayCollisionLayer = 0
			delayCollisionCounter = 0
		end
	end

end

function GetBottomGnome()
	local status = this:GetScript("GnomeStatus.lua")

	if (status.stackedBelow == nil)
	then
		return this
	else
		return status.stackedBelow:GetScript("GnomeStack.lua").GetBottomGnome()
	end
end

function UpdateParenting()
	local thisStatus = this:GetScript("GnomeStatus.lua")

	local newPos = this:GetTransform().position
	
	while(thisStatus.stacked and thisStatus.stackedAbove ~= nil)
	do
		newPos.y = newPos.y + gnomeStackDistance
		thisStatus.stackedAbove:GetTransform().position = newPos

		thisStatus.stackedAbove:GetRigidBody().velocity = vec3(0,0,0)

		thisStatus = thisStatus.stackedAbove:GetScript("GnomeStatus.lua")
	end
end

function DetachGnomes(top, bot)
	-- Get status
	local topStack = top:GetScript("GnomeStack.lua")
	local topStatus = top:GetScript("GnomeStatus.lua")
	local botStatus = bot:GetScript("GnomeStatus.lua")
	
	-- Make sure the gnomes are already attached in the way they should be
	--[[
	if (topStatus.stackedBelow ~= bot or botStatus.stackedAbove ~= top)
	then
		return nil
	end
	]]

	topStack.delayCollisionLayer = topStatus.PLAYER_PHYS_LAYER --botStatus.PLAYER_PHYS_LAYER
	topStack.delayCollisionCounter = 1
	
	topStatus.stackedBelow = nil
	botStatus.stackedAbove = nil
	
	-- Update the stacked variables.
	if (topStatus.stackedAbove ~= nil)
	then
		topStatus.stacked = true
	else
		topStatus.stacked = false
	end

	if (botStatus.stackedBelow ~= nil)
	then
		botStatus.stacked = true
	else
		botStatus.stacked = false
	end

end

function AttachGnomes(top, bot)
	-- Get status
	local topStatus = top:GetScript("GnomeStatus.lua")
	local botStatus = bot:GetScript("GnomeStatus.lua")
	

	-- Disconnect gnomes if need be.
	if (topStatus.stackedBelow ~= nil)
	then
		return
		--DetachGnomes(top, topStatus.stackedBelow)
	end
	if (botStatus.stackedAbove ~= nil)
	then
		return
		--DetachGnomes(botStatus.stackedAbove, bot)
	end

	topStatus.stackedBelow = bot
	botStatus.stackedAbove = top
	
	--SetLayersNotColliding(topStatus.PLAYER_PHYS_LAYER, botStatus.PLAYER_PHYS_LAYER)
	topStatus.stacked = true
	botStatus.stacked = true
end

function StackPlayers(other)

	-- Get positions
	local thisPos = this:GetTransform().position
	local otherPos = other:GetTransform().position
	
	if(thisPos.y < otherPos.y) --Bottom gnome handles stacking
	then
		AttachGnomes(other, this)
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

-- Disconnects from the gnome below.
function Disconnect()

	local thisStatus = this:GetScript("GnomeStatus.lua")

	DetachGnomes(this, thisStatus.stackedBelow)
end


function Unstack()
	Disconnect()

end

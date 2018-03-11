--[[
FILE: GnomeStack.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

gnomeStackDistance = 0.6

currCollisionLayer = 0
delayCollisionCounter = 0
delayCollisionLayer = 0

--gnomeColliderYSize = 1
--gnomeColliderXSize = 0.6
maxStackedDistanceX = 0.3
minStackedDistanceY = 0.3

local ATinyNumber = 0.1

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

	--ResizeColliders()
end

function EarlyUpdate(dt)
	CheckForUnstack()
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

function UpdateParenting() -- Called in GnomeMovement.lua Update because it needs to happen right after movement update
	local thisStatus = this:GetScript("GnomeStatus.lua")
	local thisTransform = this:GetTransform()

	local newPos = this:GetTransform().position

	while(thisStatus.stacked and thisStatus.stackedAbove ~= nil)
	do
		if(thisStatus.stackedBelow == nil)
		then
			thisTransform.zLayer = 4
		end 
		thisStatus.stackedAbove:GetTransform().zLayer = thisTransform.zLayer - 1

		newPos.y = newPos.y + gnomeStackDistance
		thisStatus.stackedAbove:GetTransform().position = newPos

		thisStatus.stackedAbove:GetRigidBody().velocity = vec3(0,0,0)

		thisTransform = thisStatus.stackedAbove:GetTransform()
		thisStatus = thisStatus.stackedAbove:GetScript("GnomeStatus.lua")
	end
end

--[[
Saving this for later, incase we dont like the other method of unstacking if too far

function ResizeColliders()
	local thisStatus = this:GetScript("GnomeStatus.lua")

	local numStacked = 1 -- Count this gnome

	while(thisStatus.stacked and thisStatus.stackedAbove ~= nil)
	do
		numStacked = numStacked + 1
		thisStatus = thisStatus.stackedAbove:GetScript("GnomeStatus.lua")
	end

	if(numStacked > 1 and this:GetScript("GnomeStatus.lua").stackedBelow == nil)
	then
		numStacked = numStacked + 1 -- Last gnome not accounted for in loop

		local size = gnomeColliderYSize * numStacked - gnomeStackDistance * numStacked - ATinyNumber

		this:GetCollider().dimensions = vec3(gnomeColliderXSize - ATinyNumber, size, this:GetCollider().dimensions.z)
		this:GetCollider().offset = vec3(this:GetCollider().offset.x, size / 2 - gnomeColliderYSize / 2, this:GetCollider().offset.z)
	else
		this:GetCollider().dimensions = vec3(gnomeColliderXSize, gnomeColliderYSize, this:GetCollider().dimensions.z)
		this:GetCollider().offset = vec3(this:GetCollider().offset.x, 0, this:GetCollider().offset.z)
	end
end
]]

function CheckForUnstack()
	-- Check if should unstack from bottom gnome

	local thisStatus = this:GetScript("GnomeStatus.lua")

	if(thisStatus.stackedBelow ~= nil)
	then		
		local pos = this:GetTransform().position
		local belowPos = thisStatus.stackedBelow:GetTransform().position

		local distX = math.abs(pos.x - belowPos.x)
		local distY = math.abs(pos.y - belowPos.y)

		if(distX > maxStackedDistanceX or distY < minStackedDistanceY)
		then
			print("Unstacked because " .. distX .."  " ..distY ..".    I am " ..this:GetName() ..",  was on " ..thisStatus.stackedBelow:GetName())
			print("Top was at: " ..pos.y ..", bot was at: " ..belowPos.y)

			local below = thisStatus.stackedBelow

			Unstack()
		end
		
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
	topStack.delayCollisionCounter = 0.3

	--topStack.ResizeColliders()

	topStatus.stackedBelow = nil
	botStatus.stackedAbove = nil
	PlaySound("stack_off.mp3", 0.5, 1, false)
	
	-- Update the stacked variables.
	if (topStatus.stackedAbove ~= nil)
	then
		topStatus.stacked = true
	else
		topStatus.stacked = false
		top:GetTransform().zLayer = 4
	end

	if (botStatus.stackedBelow ~= nil)
	then
		botStatus.stacked = true
	else
		botStatus.stacked = false
		bot:GetTransform().zLayer = 4
	end

end

function AttachGnomes(top, bot)
	-- Get status
	local topStack = top:GetScript("GnomeStack.lua")
	local topStatus = top:GetScript("GnomeStatus.lua")
	local botStatus = bot:GetScript("GnomeStatus.lua")

	local stackParticle = GameObject.LoadPrefab("assets/prefabs/Particles_StackEffect.json")
	local tempPos = top:GetTransform().position
	tempPos.y = tempPos.y - 1
	stackParticle:GetTransform().position = tempPos

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
	PlaySound("stack_on.mp3", 0.5, 1, false)

	top:GetTransform().zLayer = bot:GetTransform().zLayer - 1

	local newPos = bot:GetTransform().position
	while(topStatus ~= nil and topStatus.stacked)
	do
		newPos.y = newPos.y + gnomeStackDistance
		top:GetTransform().position = newPos

		top:GetScript("GnomeStack.lua").Update(0);

		if(topStatus.stackedAbove ~= nil)
		then
			top = topStatus.stackedAbove
			topStatus = topStatus.stackedAbove:GetScript("GnomeStatus.lua")
		else
			break
		end
	end

	-- Warning: top and topStatus are not the same topStatus after this loop
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
	if (other:HasTag("Player") 
		and this:GetScript("GnomeStatus.lua").stackedAbove == nil 
		and this:GetScript("GnomeStatus.lua").isStatue == false
		and other:GetScript("GnomeStatus.lua").isStatue == false)
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

--[[
FILE: FlyBoi.lua
PRIMARY AUTHOR: Brett Schiff

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

-- Variables
speed = 1

-- will trace back and forth radius in each direction if not in a circle
circleArea = false
radius = 2

-- infomation about the cone of detection
detectionConeWidth = 3.14 / 3
detectionConeLength = 3

-- delay before attacking in seconds
delayBeforeAttacking = .5
currentDelay = 0

-- editing tools
adjustCenter = true;

-- internal stuff
targetDirection = vec2(0, 0)
targetLocation = vec2(5, 5)
circleCenter = vec2(0, 0)
currMod = 1

function ChooseTarget()

    --[[ find a target direction
	local randomDirection = vec2(math.random(-1,1), math.random(-1,1))
	local length = math.sqrt((randomDirection.x * randomDirection.x) + (randomDirection.y * randomDirection.y))
	randomDirection = vec2(randomDirection.x / length, randomDirection.y / length)

	local randomDistance = math.random(1, radius)

	-- distance from the center to the target
	local dFromCToT = vec2(randomDirection.x * randomDistance, randomDirection.y * randomDistance)

	-- the location to which the bird will fly
	targetLocation = vec2(circleCenter.x + dFromCToT.x, circleCenter.y + dFromCToT.y)

	targetDirection = vec2(targetLocation.x - this:GetTransform().position.x, targetLocation.y - this:GetTransform().position.y)
	local dLength = math.sqrt((targetDirection.x * targetDirection.x) + (targetDirection.y * targetDirection.y))
	targetDirection = vec2(targetDirection.x / dLength, targetDirection.y / dLength)]]

	-- if in straight line mode
	if(circleArea == false)
	then
		-- switch which side is facing
		currMod = -1 * currMod

		-- get the target location based on a line
		targetLocation = vec2(circleCenter.x + (currMod * radius), circleCenter.y)

		-- get and normalize the target Direction
		targetDirection = vec2(targetLocation.x - this:GetTransform().position.x, targetLocation.y - this:GetTransform().position.y)
		local dLength = math.sqrt((targetDirection.x * targetDirection.x) + (targetDirection.y * targetDirection.y))
		targetDirection = vec2(targetDirection.x / dLength, targetDirection.y / dLength)

	else -- if in circle mode

		

	end
end

function MoveBoi(dt)


	local velocity = vec3(targetDirection.x * speed, targetDirection.y * speed, this:GetRigidBody().velocity.z)
	local position = this:GetTransform().position
	local destination = vec2(position.x + (velocity.x * dt), position.y + (velocity.y * dt))

	-- vectors between the destination and the target
	local desVector = vec2(destination.x - position.x, destination.y - position.y)
	local tarVector = vec2(targetLocation.x - position.x, targetLocation.y - position.y)

	-- distance to the destination and target squared
	local desDisSquared = (desVector.x * desVector.x) + (desVector.y * desVector.y)
	local tarDisSquared = (tarVector.x * tarVector.x) + (tarVector.y * tarVector.y)

	-- if the target destination is not overshot
	if(tarDisSquared > desDisSquared)
	then
		-- move towards the target using velocity
		this:GetRigidBody().velocity = velocity

	else

		--!?!? can add this back in correctly later if accuracy is super important
		--[[ stop at the target location
		this:GetTransform().position = targetLocation

		-- the rest of the flybois movement speed
		local remainingMovement = math.sqrt(math.abs(tarDisSquared)) / math.sqrt(math.abs(desDisSquared))

		-- choose the new target
		ChooseTarget()

		-- move the remaining distance towards the new target via velocity
		local velocityAdjusted = vec3(targetDirection.x * speed * remainingMovement, targetDirection.y * speed * remainingMovement, this:GetRigidBody().velocity.z)
		this:GetRigidBody().velocity = velocityAdjusted]]

		ChooseTarget()
		local newVelocity = vec3(targetDirection.x * speed, targetDirection.y * speed, this:GetRigidBody().velocity.z)
		this:GetRigidBody().velocity = newVelocity

	end

end

function EditingEffects()

	if(adjustCenter == true)
	then

		circleCenter = this:GetTransform().position
		ChooseTarget()

	end

end

-- Called at initialization
function Start()
	
	circleCenter = this:GetTransform().position

	math.randomseed(7)

	ChooseTarget()

end

-- Updates each frame
function Update(dt)
	
	EditingEffects()

	-- Move the boi toward the target, handling arriving at the target
	MoveBoi(dt)

end

-- Other is a game object
function OnCollisionEnter(other)
--[[
	if(collider.colliderData:IsCollidingWithLayer(ALLY_PROJECTILE_LAYER))
	then
		this:Destroy()
	end
]]
end

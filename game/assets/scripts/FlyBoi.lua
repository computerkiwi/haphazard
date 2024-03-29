--[[
FILE: FlyBoi.lua
PRIMARY AUTHOR: Brett Schiff

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

-- 
PLAYER1LAYER = 1 << 7
PLAYER2LAYER = 1 << 8
PLAYER3LAYER = 1 << 9
PLAYER4LAYER = 1 << 10
GROUND_LAYER = 1 << 3  --8

-- editing tools
editMode = true;

-- speeds
speed = 3
diveSpeed = 8

-- will trace back and forth radius in each direction if not in a circle
circleArea = true
radius = 4

-- infomation about the cone of detection
detectionConeWidthDegrees = 45
detectionConeLength = 3
numberOfDetectionRays = 3

-- delay before attacking in seconds
delayBeforeAttacking = .5
delayAfterDiving = .5

-- information about attack
overshootDistance = 1

-- internal stuff
currentDelay = 0
targetDirection = vec2(0, 0)
targetLocation = vec2(5, 5)
circleCenter = vec2(0, 0)
currMod = 1
isDiving = false
isOnPatrol = true

-- Health stuuuuuuuuuuuuuuff
DAMAGE_FLASH_TIME = 0.1
damageFlashTimer = 0
health = 10

local activationDistance = 10

function DegreesToRadians(degrees)

	local radiansPerDegree = 0.0174533

	return degrees * radiansPerDegree

end

function SetTargetDirection()

	-- get and normalize the target Direction
	targetDirection = vec2(targetLocation.x - this:GetTransform().position.x, targetLocation.y - this:GetTransform().position.y)
	local dLength = math.sqrt((targetDirection.x * targetDirection.x) + (targetDirection.y * targetDirection.y))
	targetDirection = vec2(targetDirection.x / dLength, targetDirection.y / dLength)

end

function SetDive(target)

	-- the bird is diving
	isDiving = true
	isOnPatrol = false

	currentDelay = delayBeforeAttacking

	targetLocation = target

	SetTargetDirection()

	targetLocation = vec2(targetLocation.x + (targetDirection.x * overshootDistance), targetLocation.y + (targetDirection.y * overshootDistance))

end

function ChooseTarget()

	-- if in straight line mode
	if(circleArea == false)
	then
		-- switch which side is facing
		currMod = -1 * currMod

		-- get the target location based on a line
		targetLocation = vec2(circleCenter.x + (currMod * radius), circleCenter.y)

		SetTargetDirection()

	else -- if in circle mode

		local randomDirectionRad = DegreesToRadians(math.random(0, 360))

		local randomDirection = vec2(math.cos(randomDirectionRad), math.sin(randomDirectionRad))

		local randomDistance = math.random() * radius

		targetLocation = vec2(circleCenter.x + (randomDistance * randomDirection.x), circleCenter.y + (randomDistance * randomDirection.y))

		SetTargetDirection()

	end
end

function MoveBoi(dt)

	local moveSpeed = speed

	if(currentDelay > 0)
	then

		moveSpeed = 0
		currentDelay = currentDelay - dt

	elseif(isDiving == true and isOnPatrol == false)
	then

		moveSpeed = diveSpeed

	end

	local velocity = vec3(targetDirection.x * moveSpeed, targetDirection.y * moveSpeed, this:GetRigidBody().velocity.z)
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
		isOnPatrol = true

	end

	-- Flip sprite
	if(targetDirection.x <= 0)
	then
		this:GetTransform().scale = vec3( math.abs(this:GetTransform().scale.x), this:GetTransform().scale.y, 1 )
	elseif(targetDirection.x > 0)
	then
		this:GetTransform().scale = vec3( -math.abs(this:GetTransform().scale.x), this:GetTransform().scale.y, 1 )
	end

end

function EditingEffects()

	if(editMode == true)
	then

		circleCenter = this:GetTransform().position
		ChooseTarget()

	end

end

local function VecSub(a, b)
  return vec2(a.x - b.x, a.y - b.y)
end

local function VecMagnitude(vec)
  return math.sqrt(vec.x * vec.x + vec.y * vec.y)
end

local function VecDistance(a, b)
  return VecMagnitude(VecSub(a,b))
end

function LookForGnomes()

	if(isOnPatrol == false)
	then

		return

	end

	-- start downward and adjust according to the cone width
	local initialDirection = 270 - (detectionConeWidthDegrees / 2)
	local directionIncrease = detectionConeWidthDegrees / (numberOfDetectionRays - 1)
	local endDirection = 270 + (detectionConeWidthDegrees / 2)
  
  --Visualisation raycast
  --Raycast.RaycastAngle(this:GetSpaceIndex(), this:GetTransform().position, initialDirection, detectionConeLength, PLAYER1LAYER | PLAYER2LAYER | PLAYER3LAYER | PLAYER4LAYER | GROUND_LAYER)
  --Raycast.RaycastAngle(this:GetSpaceIndex(), this:GetTransform().position, endDirection, detectionConeLength, PLAYER1LAYER | PLAYER2LAYER | PLAYER3LAYER | PLAYER4LAYER | GROUND_LAYER)

  local PM = _G.PLAYER_MANAGER
  for _, player in pairs(PM.PLAYERS)
  do
    local playerPos = player.gameObject:GetTransform().position
    local thisPos = this:GetTransform().position
    local offset = VecSub(playerPos, thisPos)
    local offsetAngle = (math.deg(math.atan(offset.y, offset.x)) + 360) % 360
    
    if (VecMagnitude(offset) <= detectionConeLength and offsetAngle <= endDirection and offsetAngle >= initialDirection)
    then
			SetDive(playerPos)
      break
    end
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
	
	local PM = _G.PLAYER_MANAGER
	local shortestDistance = 2 * activationDistance

	for _, player in pairs(PM.PLAYERS)
	do
		local distance = math.abs(this:GetTransform().position.x - player.gameObject:GetTransform().position.x)

		if(distance < shortestDistance)
		then

			shortestDistance = distance

		end
	end

	-- if gnomes are far away, don't update
	if(shortestDistance > activationDistance)
	then

		return

	end

	-- Damageeee
	UpdateDamageFlash(dt)
	
	-- Effects to adjust for the object being edited
	EditingEffects()
	
	-- Move the boi toward the target, handling arriving at the target
	MoveBoi(dt)
	
	-- Look for gnomes in the cone below
	LookForGnomes()

end

-- Other is a game object
function OnCollisionEnter(other)

	if(isDiving)
	then

		currentDelay = delayAfterDiving
		isDiving = false

	end

	ChooseTarget()

end


-- Kieran's stuff
function UpdateDamageFlash(dt)
	damageFlashTimer = damageFlashTimer - dt

	local sprite = this:GetSprite()
	local color = sprite.color

	if (damageFlashTimer <= 0)
	then
		color.x = 1
		color.y = 1
		color.z = 1
		color.w = 1
	else
		color.x = 0.8
		color.y = 0
		color.z = 0
		color.w = 1
	end

	sprite.color = color
end
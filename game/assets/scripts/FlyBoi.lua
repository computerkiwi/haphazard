--[[
FILE: EnemyBackAndForthMovement.lua
PRIMARY AUTHOR: Brett Schiff

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

-- Connections
otherPlayer = nil

ALLY_PROJECTILE_LAYER = 32 --1 << 5

-- Variables
speed = 1
remainingMovement = 1

-- will trace back and forth radius in each direction if not in a circle
circleArea = true
radius = 2

-- infomation about the cone of detection
detectionConeWidth = 3.14 / 3
detectionConeLength = 3

-- delay before attacking in seconds
delayBeforeAttacking = .5
currentDelay = 0

function ChooseTarget()

    -- find a target direction
	local randomDirection = vec2(math.random(0,1), math.random(0,1))
	local length = math.sqrt((randomDirection.x * randomDirection.x) + (randomDirection.y * randomDirection.y))
	randomDirection = randomDirection / length;

	local randomDistance = math.random(radius / 4, radius)


	targetLocation = circleCenter + (randomDistance * randomDirection)

	targetDirection = targetLocation - this.GetTransform().position
	local dLength = math.sqrt((targetDirection.x * targetDirection.x) + (targetDirection.y * targetDirection.y))
	targetDirection = targetDirection / dLength

end

function MoveBoi()

	local position = this.GetTransform().position
	local destination = position + targetDirection * speed

end

-- Called at initialization
function Start()
	
	circleCenter = this:GetTransform().position

	math.randomseed(7)

	ChooseTarget()

end

-- Updates each frame
function Update(dt)
	
	-- Move the boi toward the target
	MoveBoi()

end

-- Other is a game object
function OnCollisionEnter(other)

	if(this:GetDynamicCollider().colliderData:IsCollidingWithLayer(ALLY_PROJECTILE_LAYER))
	then
		this:Destroy()
	end

end

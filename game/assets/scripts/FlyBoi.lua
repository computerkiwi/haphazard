--[[
FILE: EnemyBackAndForthMovement.lua
PRIMARY AUTHOR: Brett Schiff

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

-- Connections
otherPlayer = nil

ALLY_PROJECTILE_LAYER = 32 --1 << 5

-- Variables
speed = 3

-- will trace back and forth radius in each direction if not in a circle
circleArea = true
radius = 2

-- infomation about the cone of detection
detectionConeWidth = 3.14 / 3
detectionConeLength = 3

function ChooseTarget()

    -- find a target direction
	circleCenter = this:GetTransform().position
	local randomDirection = math.random(0, 2* 3.14)
	local randomDistance = math.random(radius / 4, radius)

end

-- Called at initialization
function Start()
	
	math.randomseed(7)

	ChooseTarget()

end

-- Updates each frame
function Update(dt)
	
	-- Move the boi toward the target

	-- 

end

-- Other is a game object
function OnCollisionEnter(other)

	if(this:GetDynamicCollider().colliderData:IsCollidingWithLayer(ALLY_PROJECTILE_LAYER))
	then
		this:Destroy()
	end

end

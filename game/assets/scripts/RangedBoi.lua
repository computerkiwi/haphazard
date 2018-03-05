--[[
FILE: RangedBoi.lua
PRIMARY AUTHOR: Brett Schiff

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

-- 
PLAYER1LAYER = 1 << 7
PLAYER2LAYER = 1 << 8
PLAYER3LAYER = 1 << 9
PLAYER4LAYER = 1 << 10
GROUND_LAYER = 1 << 3

-- info about his vision
lookDirectionDegrees = 180
lookConeWidth = 45
lookConeLength = 5
lookNumRays = 5

-- info about shooting
numberOfShotsPerPattern = 3
timeBetweenShots = 1

timeBetweenFiringPatterns = 5

currTime = 0
currentlyInFiringPattern = false
shotsFired = 0

target = GameObject(0)

projectilePrefabName =  "RangedBoiProjectile.json"
projectileLaunchAngleDegrees = 45

function LookForGnomes()

	-- start downward and adjust according to the cone width
	local initialDirection = lookDirectionDegrees - (lookConeWidth / 2)
	local directionIncrease = lookConeWidth / (lookNumRays - 1)
	local endDirection = lookDirectionDegrees + (lookConeWidth / 2)

	-- raycast a number of times in the correct directions
	for currDirection = initialDirection, endDirection, directionIncrease
	do
		-- raycast from itself
		local cast = Raycast.RaycastAngle(this:GetSpaceIndex(), this:GetTransform().position, currDirection, lookConeLength, PLAYER1LAYER | PLAYER2LAYER | PLAYER3LAYER | PLAYER4LAYER | GROUND_LAYER)

		-- if a gnome was hit
		if(cast.gameObjectHit:IsValid() and cast.gameObjectHit:GetCollider().collisionLayer.layer ~= GROUND_LAYER)
		then

			print("Rangedboi: Rangedboi saw a gnome")
			currentlyInFiringPattern = true;
			currTime = timeBetweenShots
			shotsFired = 0
			return cast.gameObjectHit

		end

	end

	-- Flip sprite
	local absoluteLookDirection = math.fmod(lookDirectionDegrees , 360)

	if(absoluteLookDirection >= 90 and absoluteLookDirection < 270 )
	then
		this:GetTransform().scale = vec3( -math.abs(this:GetTransform().scale.x), this:GetTransform().scale.y, 1 )
	else
		this:GetTransform().scale = vec3( math.abs(this:GetTransform().scale.x), this:GetTransform().scale.y, 1 )
	end

	-- no gnome was found
	return GameObject(0)

end

function ShootAtTarget(target)
	
	-- spawn a projectile and get its info
	local projectile = GameObject.LoadPrefab("assets/prefabs/" .. projectilePrefabName)

	-- get the vector to the target
	local targetPosition = target:GetTransform().position
	local thisPosition = this:GetTransform().position
	local vecToTarget = vec2(targetPosition.x - thisPosition.x, targetPosition.y - thisPosition.y)
	
	-- set the position of the projectile
	local yPosition = thisPosition.y
	local xPosition = thisPosition.x
	local actualLaunchAngle = projectileLaunchAngleDegrees
	
	-- Flip sprite
	if(vecToTarget.x <= 0)
	then
		this:GetTransform().scale = vec3( -math.abs(this:GetTransform().scale.x), this:GetTransform().scale.y, 1 )
	elseif(vecToTarget.x > 0)
	then
		this:GetTransform().scale = vec3( math.abs(this:GetTransform().scale.x), this:GetTransform().scale.y, 1 )
	end

	if(vecToTarget.x <= 0)
	then

		local xOffset =  - (this:GetCollider().dimensions.x / 2) - projectile:GetCollider().dimensions.x
		xPosition = xPosition - xOffset
		actualLaunchAngle = 180 - actualLaunchAngle
		vecToTarget.x = vecToTarget.x  + xOffset

	else
	
		local xOffset = (this:GetCollider().dimensions.x / 2) + projectile:GetCollider().dimensions.x
		xPosition = xPosition - xOffset
		vecToTarget.x = vecToTarget.x  + xOffset

	end

	projectilePosition = vec2(xPosition, yPosition)
	projectile:GetTransform().position = projectilePosition

	-- set the velocity of the projectile
	local left = (1 / math.cos(math.rad(projectileLaunchAngleDegrees)))
	local numerator = ((1 / 2) * projectile:GetRigidBody().gravity.y * vecToTarget.x * vecToTarget.x)
	local denominator = (-math.abs(vecToTarget.x) * math.tan(math.rad(projectileLaunchAngleDegrees)) + vecToTarget.y)
	local right = math.sqrt(math.abs(numerator / denominator))
	speed = math.abs(left * right)

	directionX = (math.cos(math.rad(actualLaunchAngle)))
	directionY = (math.sin(math.rad(actualLaunchAngle)))

	local direction = vec3(directionX, directionY, 0)

	print("Speed: " .. tostring(speed))
	print("Direction: " .. tostring(direction.x) .. "      " .. tostring(direction.y))

	projectile:GetRigidBody().velocity = vec3(direction.x * speed, direction.y * speed, projectile:GetRigidBody().velocity.z )

end

function ShootAtCorrectTime(dt)

	-- if the enemy is between firing patterns
	if(currentlyInFiringPattern == false)
	then
		-- if the time has passed to go back into a firing pattern
		if(currTime >= timeBetweenFiringPatterns)
		then
			
			currentlyInFiringPattern = true
			shotsFired = 0
			currTime = currTime - timeBetweenFiringPatterns + timeBetweenShots
			return

		else -- continue waiting till the next firing pattern

			currTime = currTime + dt
			return

		end

	else -- the enemy is in a firing pattern

		if(currTime >= timeBetweenShots)
		then

			currTime = currTime - timeBetweenShots
			shotsFired = shotsFired + 1

			-- if the firing pattern has been completed, break
			if(shotsFired >= numberOfShotsPerPattern)
			then

				currentlyInFiringPattern = false
				shotsFired = 0

			end

		else -- continue waiting till the next shot

			currTime = currTime + dt
			return

		end
	end

	-- Raycast at the gnome to make sure it's still in vision
	directionTowardsTarget = vec2(- this:GetTransform().position.x + target:GetTransform().position.x, - this:GetTransform().position.y + target:GetTransform().position.y)

	theCast = Raycast.Cast(this:GetSpaceIndex(), this:GetTransform().position, directionTowardsTarget, lookConeLength, PLAYER1LAYER | PLAYER2LAYER | PLAYER3LAYER | PLAYER4LAYER | GROUND_LAYER)

	if(theCast.gameObjectHit:IsValid() and theCast.gameObjectHit:GetCollider().collisionLayer.layer ~= GROUND_LAYER)
	then
		
		ShootAtTarget(theCast.gameObjectHit)

	else

		currentlyInFiringPattern = false
		currTime = timeBetweenShots
		shotsFired = 0
		target = GameObject(0)

	end

end


-- Called at initialization
function Start()
	
	

end

-- Updates each frame
function Update(dt)
	
	-- if there is no valid target
	if(not target:IsValid())
	then

		-- look for gnomes
		target = LookForGnomes()

	else

		-- shoot at the target gnome
		ShootAtCorrectTime(dt)

	end
	

	

end

-- Other is a game object
function OnCollisionEnter(other)

	

end

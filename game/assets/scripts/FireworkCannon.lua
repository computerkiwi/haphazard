--[[
FILE: Firework.lua
PRIMARY AUTHOR: Brett Schiff

Copyright (c) 2017 DigiPen (USA) Corporation.
]]




timeToSpawnBoomMin = .5
timeToSpawnBoomMax = 2

timeToDelete = 9

xVelocityMin = -2
xVelocityMax = 2

yVelocityMin = 5
yVelocityMax = 8

fireworkGravity = -3

timeBetweenShots = 1
shotsPerRound = 1

color = 0

redPrefab = "assets/prefabs/fireworks_internal/firework_fireworkred.json"
yellowPrefab = "assets/prefabs/fireworks_internal/firework_fireworkyellow.json"
greenPrefab = "assets/prefabs/fireworks_internal/firework_fireworkgreen.json"
bluePrefab = "assets/prefabs/fireworks_internal/firework_fireworkblue.json"

currTime = 0

function SpawnObjectAtObject(prefabName, parentObj)

	local obj = GameObject.LoadPrefab(prefabName)

	if (obj:IsValid())
	then

		obj:GetTransform().position = parentObj:GetTransform().position

		return obj
	else
		return nil
	end
end

function SpawnRandomizedFirework()

	local xVel = xVelocityMin + (math.random() * (xVelocityMax - xVelocityMin))
	local yVel = yVelocityMin + (math.random() * (yVelocityMax - yVelocityMin))

	local timeToBoom = timeToSpawnBoomMin + (math.random() * (timeToSpawnBoomMax - timeToSpawnBoomMin))

	color = color + 1

	if(color > 3)
	then

		color = 0

	end

	local fireworkObj = GameObject(0)

	if(color == 0)
	then

		fireworkObj = SpawnObjectAtObject(redPrefab, this)

	elseif(color == 1)
	then

		fireworkObj = SpawnObjectAtObject(yellowPrefab, this)

	elseif(color == 2)
	then

		fireworkObj = SpawnObjectAtObject(greenPrefab, this)

	elseif(color == 3)
	then

		fireworkObj = SpawnObjectAtObject(bluePrefab, this)

	end

	fireworkObj:GetRigidBody().velocity = vec3(xVel, yVel, 0)
	fireworkObj:GetRigidBody().gravity = vec3(0, fireworkGravity, 0)

	local fireworkScript = fireworkObj:GetScript("Firework.lua")

	fireworkScript.timeToSpawnBoom = timeToBoom

end

function Update(dt)

	currTime = currTime + dt
	
	if(currTime >= timeBetweenShots)
	then

		for i = 0, shotsPerRound - 1, 1
		do

			SpawnRandomizedFirework()

		end

		currTime = currTime - timeBetweenShots

	end
end
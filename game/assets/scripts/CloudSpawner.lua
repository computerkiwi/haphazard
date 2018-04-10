--[[
FILE: CloudSpawner.lua
PRIMARY AUTHOR: Brett Schiff

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

local cloudPrefabs = {"assets/prefabs/clouds_internal/cloud1.json", "assets/prefabs/clouds_internal/cloud2.json", "assets/prefabs/clouds_internal/cloud3.json"}
local numPrefabs = 3

minSpawnTime = 3
maxSpawnTime = 5

minVelocityX = 1
maxVelocityX = 1.5

spawnRadius = 5

timeTillNextSpawn = 0

timeToDestroyClouds = 60

index = 1

function SpawnObjectAtObjectWithVelocityandYOffset(prefabName, parentObj, velocity, yOffset)

	local obj = GameObject.LoadPrefab(prefabName)

	if (obj:IsValid())
	then

		obj:GetTransform().position = vec2(parentObj:GetTransform().position.x, parentObj:GetTransform().position.y + yOffset)
		obj:GetRigidBody().velocity = velocity

		return obj
	else
		return nil
	end
end

function SpawnRandomizedCloud()

	local xVel = minVelocityX + (math.random() * (maxVelocityX - minVelocityX))
	local velocity = vec3(xVel, 0, 0)
	local yOffset = -spawnRadius + (math.random() * (2 * spawnRadius))

	local prefab = cloudPrefabs[index]

	local obj = SpawnObjectAtObjectWithVelocityandYOffset(prefab, this, velocity, yOffset)

	obj:GetScript("DestroyAfterTime.lua").TIME_TO_DELETE = timeToDestroyClouds

	index = index + 1
	if(index > numPrefabs)
	then

		index = 1

	end

	return obj

end

function PreSeedClouds(time)

	local remainingTime = time

	while remainingTime > 0
	do
		
		obj = SpawnRandomizedCloud()

		-- account for the missing time
		obj:GetTransform().position = vec2(obj:GetTransform().position.x + (obj:GetRigidBody().velocity.x * remainingTime), obj:GetTransform().position.y)
		obj:GetScript("DestroyAfterTime.lua").TIME_TO_DELETE = obj:GetScript("DestroyAfterTime.lua").TIME_TO_DELETE - remainingTime

		-- skip the the next time a cloud would spawn
		timeTillNextSpawn = minSpawnTime + (math.random() * (maxSpawnTime - minSpawnTime))
		remainingTime = remainingTime - timeTillNextSpawn

	end

end

function Start()

	PreSeedClouds(timeToDestroyClouds)

end

function Update(dt)

	timeTillNextSpawn = timeTillNextSpawn - dt

	if(timeTillNextSpawn <= 0)
	then

		SpawnRandomizedCloud()

		timeTillNextSpawn = minSpawnTime + (math.random() * (maxSpawnTime - minSpawnTime))

	end

end
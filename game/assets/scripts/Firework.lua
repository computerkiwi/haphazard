--[[
FILE: Firework.lua
PRIMARY AUTHOR: Brett Schiff

Copyright (c) 2017 DigiPen (USA) Corporation.
]]




timeToSpawnBoom = 1.5
timeToDelete = 3

fireworkGravity = -3

sparklerPrefab = "assets/prefabs/fireworks_internal/firework_trail.json"
boomPrefab = "assets/prefabs/fireworks_internal/firework_greenboom.json"

sparkleObj = nil
boomObj = nil
boomObj2 = nil

currTime = 0

function SpawnAndAttachObject(prefabName, parentObj)

	local obj = GameObject.LoadPrefab(prefabName)

	if (obj:IsValid())
	then

		obj:GetTransform().position = parentObj:GetTransform().position
		obj:GetTransform().parent = parentObj

		return obj
	else
		return nil
	end
end

function Start()

	-- spawn the sparkler object
	sparkleObj = SpawnAndAttachObject(sparklerPrefab, this)

end

function Update(dt)

	currTime = currTime + dt
	
	if(currTime >= timeToSpawnBoom and sparkleObj:IsValid())
	then

		-- destroy the sparkler object
		sparkleObj:Destroy()
		sparkleObj = GameObject(0)

		-- spawn the boom object
		boomObj = SpawnAndAttachObject(boomPrefab, this)
		boomObj:GetTransform().position = this:GetTransform().position
		boomObj2 = SpawnAndAttachObject(boomPrefab, this)
		boomObj2:GetTransform().position = this:GetTransform().position

	end

	if (currTime >= timeToDelete)
	then
		
		-- destroy the boom object
		boomObj:Destroy()
		boomObj2:Destroy()

		this:Destroy()

	end
end
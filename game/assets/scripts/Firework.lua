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

function RandomFloat(a, b)
	return math.random() * (b - a) + a
end

function NewPlaysoundFunction(...)
  local sounds = {...}
  return function(volume)
    local soundName = sounds[math.random(1, #sounds)]
	
    PlaySound(soundName, volume, 1 + RandomFloat(-0.1, 0.1), false)
  end
end

PlayBoomSound = NewPlaysoundFunction(
  "explosion_01.wav",
  "explosion_02.wav")


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
		
		if (math.random() < 0.5)
		then
			PlayBoomSound(1)
		else
			PlayBoomSound(0.4)
		end

	end

	if (currTime >= timeToDelete)
	then
		
		-- destroy the boom object
		boomObj:Destroy()
		boomObj2:Destroy()

		this:Destroy()

	end
end
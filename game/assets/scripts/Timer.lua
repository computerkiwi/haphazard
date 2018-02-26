--[[
FILE: Timer.lua
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

internalTimerObj = nil

startingTime = 60
maxTime = 80

star_1 = 10
star_2 = 20
star_3 = 40

-- Use this variable from the editor to reset the timer.
resetTimer = false



function SpawnInternalTimer()
	local obj = GameObject.LoadPrefab("assets/prefabs/InternalTimer.json")
	if (obj:IsValid())
	then
		obj:GetTransform().parent = this
	
		return obj
	else
		return nil
	end
end

-- amount ranges from 0 to 1
function SetInternalVisual(amount)
	local thisTransform = this:GetTransform()
	local internalTransform = internalTimerObj:GetTransform()
	
	-- Internal transform should be parented to us so set the local position.
	internalTransform.localPosition = vec2((thisTransform.scale.x / 2) * (1 - amount), 0)
	
	local tempScale = thisTransform.scale
	tempScale.x = tempScale.x * amount
	internalTransform.scale = tempScale
end

function ResetTimer()
	timer = startingTime
end

function Start()
	ResetTimer()
	internalTimerObj = SpawnInternalTimer()
end

function Update(dt)
	-- Reset the timer from the editor.
	if (resetTimer)
	then
		resetTimer = false
		ResetTimer()
	end
	
	-- Tick the timer down
	timer = math.max(timer - dt, 0)
	SetInternalVisual(timer / maxTime)
end

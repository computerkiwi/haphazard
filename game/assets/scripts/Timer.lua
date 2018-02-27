--[[
FILE: Timer.lua
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

internalTimerObj = nil
stars = {}

startingTime = 60
maxTime = 80

star_1 = 10
star_2 = 20
star_3 = 40

starHeight = 1

-- We can't set arrays from the editor, so we manually put them into a table for iteration
starTimes = {}

STAR_SPRITE_ACTIVE = "Sticker_ChallengeGem.png"
STAR_SPRITE_MISSED = "Sticker_Gem.png"
STAR_COUNT = 3

-- Use this variable from the editor to reset the timer.
resetTimer = false

function AddTime(addTime)
	timer = math.max(math.min(timer + addTime, maxTime), 0)
end

function SpawnAndAttachObject(prefabName)
	local obj = GameObject.LoadPrefab(prefabName)
	if (obj:IsValid())
	then
		obj:GetTransform().parent = this
	
		return obj
	else
		return nil
	end
end

function SpawnInternalTimer()
	return SpawnAndAttachObject("assets/prefabs/timer_internal/InternalTimer.json")
end

function SpawnStar()
	return SpawnAndAttachObject("assets/prefabs/timer_internal/TimerStar.json")
end

-- amount ranges from 0 to 1
function AmountToLocalXPos(amount)
	local thisTransform = this:GetTransform()
	
	return ((thisTransform.scale.x) * -(amount - 0.5))
end

function SetInternalVisual()
	local amount = timer / maxTime
	local thisTransform = this:GetTransform()
	local internalTransform = internalTimerObj:GetTransform()
	
	-- Internal transform should be parented to us so set the local position.
	internalTransform.localPosition = vec2(AmountToLocalXPos(amount / 2), 0)
	
	local tempScale = thisTransform.scale
	tempScale.x = tempScale.x * amount
	internalTransform.scale = tempScale
	
	-- Handle the stars
	starTimes[1] = star_1
	starTimes[2] = star_2
	starTimes[3] = star_3
	tempScale.y = tempScale.y * starHeight
	tempScale.x = tempScale.y -- Stars are square.
	for i = 1,STAR_COUNT
	do
		local star = stars[i]
		if (star ~= nil)
		then
			local starAmount = starTimes[i] / maxTime
			local starTransform = star:GetTransform()
			starTransform.localPosition = vec2(AmountToLocalXPos(starAmount), 0)
			starTransform.scale = tempScale
			
			if (starTimes[i] <= timer)
			then
				star:GetSprite().id = Resource.FilenameToID(STAR_SPRITE_ACTIVE)
			else
				star:GetSprite().id = Resource.FilenameToID(STAR_SPRITE_MISSED)
			end
		end
	end
end

function ResetTimer()
	timer = startingTime
end

function Start()
	ResetTimer()
	internalTimerObj = SpawnInternalTimer()
	
	for i = 1, STAR_COUNT
	do
		stars[i] = SpawnStar()
	end
	
	_G.AddTimeToTimer = AddTime
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

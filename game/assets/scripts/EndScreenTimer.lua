--[[
FILE: EndScreenTimer.lua
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

internalTimerObj = nil
stars = {}

starHeight = 1

STAR_SPRITE_ACTIVE = "Sticker_ChallengeGem.png"
STAR_SPRITE_MISSED = "Sticker_Gem.png"
STAR_COUNT = 3

-- Also set up in Timer.lua
if (_G.globalEndScreenTable == nil)
then
	_G.globalEndScreenTable = {starTimes = {2, 3, 4}, finishTime = 9, maxTime = 10, nextLevel = "defaultLevel.json"}
end
endTable = _G.globalEndScreenTable

timer = 0
timeToMax = 5

function ContinuePressed()
	-- Spacebar
	if (OnPress(KEY.Space))
	then
		return true
	end
	
	-- A Button on the gamepads
	for player = 0,3
	do
		local A_BUTTON = 0
		if (GamepadOnPress(player, A_BUTTON))
		then
			return true
		end
	end
	
	-- Failed to find a continue.
	return false
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
	local amount = timer / endTable.maxTime
	local thisTransform = this:GetTransform()
	local internalTransform = internalTimerObj:GetTransform()
	
	-- Internal transform should be parented to us so set the local position.
	internalTransform.localPosition = vec2(AmountToLocalXPos(amount / 2), 0)
	
	local tempScale = thisTransform.scale
	tempScale.x = tempScale.x * amount
	internalTransform.scale = tempScale
	
	-- Handle the stars
	tempScale.y = tempScale.y * starHeight
	tempScale.x = tempScale.y -- Stars are square.
	for starNum, starTime in pairs(endTable.starTimes)
	do
		local star = stars[starNum]
		if (star ~= nil)
		then
			local starAmount = starTime / endTable.maxTime
			local starTransform = star:GetTransform()
			starTransform.localPosition = vec2(AmountToLocalXPos(starAmount), 0)
			starTransform.scale = tempScale
			
			if (starTime <= timer)
			then
				star:GetSprite().id = Resource.FilenameToID(STAR_SPRITE_ACTIVE)
			else
				star:GetSprite().id = Resource.FilenameToID(STAR_SPRITE_MISSED)
			end
		end
	end
end

function Start()
	internalTimerObj = SpawnInternalTimer()
	for i = 1, STAR_COUNT
	do
		stars[i] = SpawnStar()
	end
	
	timerSpeed = endTable.maxTime / timeToMax
end

function Update(dt)	
	if (timer < endTable.finishTime)
	then
		timer = timer + timerSpeed * dt
	else
		timer = endTable.finishTime
		if (ContinuePressed())
		then
			Engine.LoadLevel(endTable.nextLevel)
		end
	end

	SetInternalVisual()
end

--[[
FILE: CameraFollow.lua
PRIMARY AUTHOR: Kieran

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

local LERP_SPEED_X = 0.05
local LERP_SPEED_Y = 0.05
local LERP_SPEED_ZOOM = 0.05

MIN_WIDTH = 4
MAX_WIDTH = 10

Y_OFFSET = 2
Y_BUFFER = 2
X_BUFFER = 10

useFocus = true

local camPos = {x = 0, y = 0}

local MAX_SHAKE_X = 1.2
local MAX_SHAKE_Y = 1.2
local MAX_SHAKE_ROT = 12
local SHAKE_DECAY_RATE = 1
local trauma = 0
local traumaChange = 0

-- Takes a pair of vec2
function VectorDistance(a, b)
	local x = b.x - a.x
	local y = b.y - a.y

	return math.sqrt(x * x + y * y)
end

local function RandomFloat(a, b)
	return (b - a) * math.random() + a
end

function AddTrauma(amount)
	-- Only apply the largest screenshake amount each frame.
	traumaChange = math.max(traumaChange, amount)
end

function Start()
	-- Make the camera shake function accessible
	_G.Screenshake = AddTrauma
	
	local transformPos = this:GetTransform().position
	camPos.x = transformPos.x
	camPos.y = transformPos.y
end

function SetCameraBounds(left, right, top, bot)
	local centerX = (left + right) / 2
	local centerY = (top + bot) / 2
	
	local width = right - left
	local height = top - bot
	
	-- Lerp toward the center position.
	camPos.x = math.lerp(camPos.x, centerX, LERP_SPEED_X)
	camPos.y = math.lerp(camPos.y, centerY, LERP_SPEED_Y)

	-- Zoom based on the distance.
	local camera = this:GetCamera()
	
	-- Zoom to keep X and Y onscreen (0.6 is an experimentally-discovered multiplier)
	local zoomX = width * 0.6
	local zoomY = height * 1.0
	
	-- Zoom to the higher of the values.
	local zoomVal = math.max(zoomX, zoomY)
	camera.zoom = math.lerp(camera.zoom, zoomVal, LERP_SPEED_ZOOM)
end

-- Set the real position, taking screenshake into account.
function UpdateRealPosition(dt)
	trauma = math.min(math.max(0, trauma + traumaChange), 1)
	traumaChange = 0

	local shakeX = (trauma * trauma) * RandomFloat(-MAX_SHAKE_X, MAX_SHAKE_X)
	local shakeY = (trauma * trauma) * RandomFloat(-MAX_SHAKE_Y, MAX_SHAKE_Y)
	local shakeRot = (trauma * trauma) * RandomFloat(-MAX_SHAKE_ROT, MAX_SHAKE_ROT)

	this:GetTransform().position = vec2(camPos.x + shakeX, camPos.y + shakeY)
	this:GetTransform().rotation = shakeRot
	
	trauma = math.min(math.max(0, trauma - SHAKE_DECAY_RATE * dt), 1)
end

function ThresholdFunc(distance, innerTresh, outerTresh)
	if (distance > outerTresh)
	then
		return 0
	elseif (distance < innerTresh)
	then
		return 1
	else
		return math.lerp(1, 0, (distance - innerTresh) / (outerTresh - innerTresh))
	end
end

-- Returns newLeft, newRight, newTop, newBot
function GetFocusChange(left, right, top, bot, focusPos, innerTresh, outerTresh, leftMult, rightMult, topMult, botMult)
	local x = focusPos.x
	local y = focusPos.y

	if (x > right)
	then
		if (y > top)
		then
			-- CASE: TOP_RIGHT
			distance = VectorDistance(focusPos, {x = right, y = top})
			right = math.lerp(right, x, ThresholdFunc(distance * rightMult, innerTresh, outerTresh))
			top = math.lerp(top, y, ThresholdFunc(distance * topMult, innerTresh, outerTresh))
		elseif (y < bot)
		then
			-- CASE: BOT_RIGHT
			distance = VectorDistance(focusPos, {x = right, y = bot})
			right = math.lerp(right, x, ThresholdFunc(distance * rightMult, innerTresh, outerTresh))
			bot = math.lerp(bot, y, ThresholdFunc(distance * botMult, innerTresh, outerTresh))
		else
			-- CASE: RIGHT
			local distance = x - right
			right = math.lerp(right, x, ThresholdFunc(distance * rightMult, innerTresh, outerTresh))
		end
	elseif (x < left)
	then
		if (y > top)
		then
			-- CASE: TOP_LEFT
			distance = VectorDistance(focusPos, {x = left, y = top})
			left = math.lerp(left, x, ThresholdFunc(distance * leftMult, innerTresh, outerTresh))
			top = math.lerp(top, y, ThresholdFunc(distance * topMult, innerTresh, outerTresh))
		elseif (y < bot)
		then
			-- CASE: BOT_LEFT
			distance = VectorDistance(focusPos, {x = left, y = bot})
			left = math.lerp(left, x, ThresholdFunc(distance * leftMult, innerTresh, outerTresh))
			bot = math.lerp(bot, y, ThresholdFunc(distance * botMult, innerTresh, outerTresh))
		else
			-- CASE: LEFT
			local distance = left - x
			left = math.lerp(left, x, ThresholdFunc(distance * leftMult, innerTresh, outerTresh))
		end
	else
		if (y > top)
		then
			-- CASE: TOP
			local distance = y - top
			top = math.lerp(top, y, ThresholdFunc(distance * topMult, innerTresh, outerTresh))
		elseif (y < bot)
		then
			-- CASE: BOT
			local distance = bot - y
			bot = math.lerp(bot, y, ThresholdFunc(distance * botMult, innerTresh, outerTresh))
		else
			-- CASE: INSIDE
			-- Don't change anything
		end
	end
	
	return left, right, top, bot
end

function Update(dt)
	local PM = _G.PLAYER_MANAGER
	
	-- Gnome target position is the average x between the furthest extremes of the gnome positions
	local minGnomeX = math.huge
	local maxGnomeX = -math.huge
	local minGnomeY = math.huge
	local maxGnomeY = -math.huge
	for k,player in pairs(PM.PLAYERS)
	do
		local gnomeObj = player.gameObject
		if (gnomeObj:IsValid())
		then
			local posX = gnomeObj:GetTransform().position.x
			minGnomeX = math.min(posX, minGnomeX)
			maxGnomeX = math.max(posX, maxGnomeX)
			
			local posY = gnomeObj:GetTransform().position.y
			minGnomeY = math.min(posY, minGnomeY)
			maxGnomeY = math.max(posY, maxGnomeY)
		end
	end
	
	maxGnomeY = maxGnomeY + Y_OFFSET
	
	-- Keep the camera within width bounds.
	local gnomeWidth = maxGnomeX - minGnomeX
	local amountOver = gnomeWidth - MAX_WIDTH
	if (amountOver > 0)
	then
		maxGnomeX = maxGnomeX - amountOver / 2
		minGnomeX = minGnomeX + amountOver / 2
	elseif(gnomeWidth < MIN_WIDTH)
	then
		local center = (minGnomeX + maxGnomeX) / 2
		minGnomeX = center - MIN_WIDTH / 2
		maxGnomeX = center + MIN_WIDTH / 2
	end
	
	-- Vars to store focused bounds.
	local left = minGnomeX 
	local right = maxGnomeX
	local top = maxGnomeY
	local bot = minGnomeY

	-- Take focus objects into account.
	if (useFocus and _G.focusObjects)
	then
		-- Loop through and get the widest bounds CameraFocus objects are asking for.
		for _, focusObj in pairs(_G.focusObjects)
		do
			if (focusObj:IsValid())
			then
				local focusObjPos = focusObj:GetTransform().position
				local focusScript = focusObj:GetScript("CameraFocus.lua")
				if (focusScript)
				then
					local newLeft
					local newRight
					local newTop
					local newBot
					
					newLeft, newRight, newTop, newBot = GetFocusChange(minGnomeX, maxGnomeX, maxGnomeY, minGnomeY, focusObjPos, 
																	   focusScript._INNER_THRESHOLD, focusScript._OUTER_THRESHOLD,
																	   focusScript.LEFT_DISTANCE_MULTIPLIER,
																	   focusScript.RIGHT_DISTANCE_MULTIPLIER,
																	   focusScript.TOP_DISTANCE_MULTIPLIER,
																	   focusScript.BOT_DISTANCE_MULTIPLIER)
					
					left = math.min(left, newLeft)
					right = math.max(right, newRight)
					top = math.max(top, newTop)
					bot = math.min(bot, newBot)
				end
			end
		end
	end
		
	-- Shrink in the sides a bit if we went over max width. If we already went over earlier just use gnome bounds for x.
	if (amountOver <= 0)
	then
		local width = right - left
		local focusOver = width - MAX_WIDTH
		if (focusOver > 0)
		then
			local focusChange = width - gnomeWidth
			local focusMultiplier = math.max(0, (MAX_WIDTH - gnomeWidth) / focusChange)
			
			left = math.lerp(minGnomeX, left, focusMultiplier)
			right = math.lerp(maxGnomeX, right, focusMultiplier)
		end
	else
		left = minGnomeX
		right = maxGnomeX
	end
	
	-- Add the buffer
	local left = left - X_BUFFER / 2
	local right = right + X_BUFFER / 2
	local top = top + Y_BUFFER / 2
    local bot = bot - Y_BUFFER / 2
	
	-- Screenshake debugging.
	if (OnPress(KEY.P))
	then
		AddTrauma(0.3333)
	end
	--]]
	SetCameraBounds(left, right, top, bot)
	UpdateRealPosition(dt)
	
	_G.focusObjects = {}
end

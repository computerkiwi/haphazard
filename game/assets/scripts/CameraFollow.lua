--[[
FILE: CameraFollow.lua
PRIMARY AUTHOR: Kieran

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

local LERP_SPEED_X = 0.05
local LERP_SPEED_Y = 0.05
local LERP_SPEED_ZOOM = 0.05

MAX_WIDTH = 10

Y_OFFSET = 2
Y_BUFFER = 2
X_BUFFER = 10

useFocus = true

-- Takes a pair of vec2
function VectorDistance(a, b)
	local x = b.x - a.x
	local y = b.y - a.y

	return math.sqrt(x * x + y * y)
end

function SetCameraBounds(left, right, top, bot)
	local centerX = (left + right) / 2
	local centerY = (top + bot) / 2
	
	local width = right - left
	local height = top - bot
	
	-- Get the transform/position of the camera.
	local transform = this:GetTransform()
	local currPos = transform.position
	
	-- Lerp toward the center position.
	currPos.x = math.lerp(currPos.x, centerX, LERP_SPEED_X)
	currPos.y = math.lerp(currPos.y, centerY, LERP_SPEED_Y)
	transform.position = currPos

	-- Zoom based on the distance.
	local camera = this:GetCamera()
	
	-- Zoom to keep X and Y onscreen (0.6 is an experimentally-discovered multiplier)
	local zoomX = width * 0.6
	local zoomY = height * 1.0
	
	-- Zoom to the higher of the values.
	local zoomVal = math.max(zoomX, zoomY)
	camera.zoom = math.lerp(camera.zoom, zoomVal, LERP_SPEED_ZOOM)
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
	
	-- Keep the camera under max width.
	local gnomeWidth = maxGnomeX - minGnomeX
	local amountOver = gnomeWidth - MAX_WIDTH
	if (amountOver > 0)
	then
		maxGnomeX = maxGnomeX - amountOver / 2
		minGnomeX = minGnomeX + amountOver / 2
	end
	
	-- Vars to store focused bounds.
	local left = minGnomeX 
	local right = maxGnomeX
	local top = maxGnomeY
	local bot = minGnomeY

	-- Take focus objects into account if we're not already at the max.
	if (amountOver <= 0)
	then
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
		
		-- Shrink in the sides a bit if we went over max width.
		local width = right - left
		local focusOver = width - MAX_WIDTH
		if (focusOver > 0)
		then
			local focusChange = width - gnomeWidth
			local focusMultiplier = math.max(0, (MAX_WIDTH - gnomeWidth) / focusChange)
			
			left = math.lerp(minGnomeX, left, focusMultiplier)
			right = math.lerp(maxGnomeX, right, focusMultiplier)
		end
	end
	
	-- Add the buffer
	local left = left - X_BUFFER / 2
	local right = right + X_BUFFER / 2
	local top = top + Y_BUFFER / 2
    local bot = bot - Y_BUFFER / 2
	
	SetCameraBounds(left, right, top, bot)	
	
	_G.focusObjects = {}
end

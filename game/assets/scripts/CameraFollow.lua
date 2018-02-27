--[[
FILE: CameraFollow.lua
PRIMARY AUTHOR: Kieran

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

LERP_SPEED = 0.05

ZOOM_SCALE = 1

MIN_ZOOM = 10
MaxZoom = 20
dist = 1

OFFSET_Y = 0.05

-- Takes a pair of vec2
function VectorDistance(a, b)
	local x = b.x - a.x
	local y = b.y - a.y

	return math.sqrt(x * x + y * y)
end

function max(a, b)
	if (a > b)
	then
		return a
	else
		return b
	end
	
end

function Start()
	OFFSET_Y = 0.5
end

function Update(dt)
	local PM = _G.PLAYER_MANAGER
	
	-- Get gnome positions and average.
	local gnomeCount = 0
	local positions = {}
	local avgPos = vec2(0, 0)
	for k,player in pairs(PM.PLAYERS)
	do
		local gnomeObj = player.gameObject
		if (gnomeObj:IsValid())
		then
			gnomeCount = gnomeCount + 1
			local transform = gnomeObj:GetTransform()
			table.insert(positions, transform.position)
			avgPos.x = avgPos.x + transform.position.x
			avgPos.y = avgPos.y + transform.position.y
		end
	end
	avgPos.x = avgPos.x / gnomeCount
	avgPos.y = avgPos.y / gnomeCount
	
	-- Get the transform/position of the camera.
	local transform = this:GetTransform()
	local currPos = transform.position
	
	-- Exit early for no gnomes.
	if (gnomeCount == 0)
	then
		return nil
	end
	
	-- Lerp toward the average gnome position.
	currPos.x = math.lerp(currPos.x, avgPos.x, LERP_SPEED)
	currPos.y = math.lerp(currPos.y, avgPos.y + OFFSET_Y, LERP_SPEED)
	transform.position = currPos
	
	-- Get the furthest gnome distance from the camera
	local maxGnomeDist = 0
	for gnomeNum, gnomePos in pairs(positions)
	do
		local currentDist = VectorDistance(avgPos, gnomePos)
		maxGnomeDist = math.max(maxGnomeDist, currentDist)
	end
	
	-- Zoom based on the distance.
	local camera = this:GetCamera()
	dist = maxGnomeDist * 2
	camera.zoom = math.lerp(camera.zoom, ClampedZoom(), LERP_SPEED)
end

function ClampedZoom()

	local value = dist

	if value < MIN_ZOOM
	then
		value = MIN_ZOOM
	elseif value > MaxZoom
	then
		value = MaxZoom
	end

	return value
end
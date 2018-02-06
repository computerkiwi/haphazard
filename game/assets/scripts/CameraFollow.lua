--[[
FILE: CameraFollow.lua
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
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

function Update(dt)
	local player1 = GameObject.FindByName("Player1")
	local player2 = GameObject.FindByName("Player2")
	local camera = this:GetCamera()

	-- Check if we actually got the players.
	if (not player1:IsValid() or not player2:IsValid())
	then
		return
	end

	local position = player1:GetTransform().position
	local otherPos = player2:GetTransform().position
	
	position.x = (position.x + otherPos.x) / 2
	position.y = (position.y + otherPos.y) / 2
	
	local transform = this:GetTransform()
	local currPos = transform.position
	
	currPos.x = math.lerp(currPos.x, position.x, LERP_SPEED)
	currPos.y = math.lerp(currPos.y, position.y + OFFSET_Y, LERP_SPEED)
	
	transform.position = currPos
	
	dist = VectorDistance(player1:GetTransform().position, player2:GetTransform().position)
	camera.zoom = ClampedZoom()
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
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
	local player3 = GameObject.FindByName("Player3")
	local player4 = GameObject.FindByName("Player4")

	local camera = this:GetCamera()

	-- Check if we actually got the players.
	if (not player1:IsValid() or not player2:IsValid())
	then
		return
	end

	local position = this:GetTransform().position

	local position1 = player1:GetTransform().position
	local position2 = player2:GetTransform().position
	local position3 = player3:GetTransform().position
	local position4 = player4:GetTransform().position

	position.x = (position1.x + position2.x + position3.x + position4.x) / 4
	position.y = (position1.y + position2.y + position3.y + position4.y) / 4

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
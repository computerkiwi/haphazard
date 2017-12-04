--[[
FILE: CameraFollow.lua
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

LERP_SPEED = 0.05

ZOOM_SCALE = 1

MIN_ZOOM = 5

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
	
	local position = player1:GetTransform().position
	local otherPos = player2:GetTransform().position
	
	position.x = (position.x + otherPos.x) / 2
	position.y = (position.y + otherPos.y) / 2
	
	local transform = this:GetTransform()
	local currPos = transform.position
	
	currPos.x = ((1 - LERP_SPEED) * currPos.x + LERP_SPEED * position.x)
	currPos.y = ((1 - LERP_SPEED) * currPos.y + LERP_SPEED * position.y)
	
	transform.position = currPos
	
	local dist = VectorDistance(player1:GetTransform().position, player2:GetTransform().position)
	local camera = this:GetCamera()
	camera.zoom = max(MIN_ZOOM, ((1 - LERP_SPEED) * camera.zoom + LERP_SPEED * dist * ZOOM_SCALE))
	
	
	
end

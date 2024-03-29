--[[
FILE: EnemyBackAndForthMovement.lua
PRIMARY AUTHOR: Brett Schiff

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

-- Connections
otherPlayer = nil

GROUND_LAYER = 1 << 3

-- Variables
direction = 1 -- direction of movement - -1 for left, 1 for right
speed = 2      -- speed at which boi moves

local TurnAroundTimer = .2
local currTime = 0

-- Updates each frame
function Update(dt)
	
	local velocity = this:GetRigidBody().velocity

	cast = Raycast.Cast(this:GetSpaceIndex(), this:GetTransform().position, vec2(0, -1), this:GetCollider().dimensions.y * (3/4), GROUND_LAYER)

	currTime = currTime + dt

	if((velocity.x == 0 or not cast.gameObjectHit:IsValid()) and currTime >= TurnAroundTimer)
	then
		
		currTime = 0

		-- change direction
		direction = -direction

		-- Flip sprite
		if(direction == -1)
		then
			this:GetTransform().scale = vec3( -math.abs(this:GetTransform().scale.x), this:GetTransform().scale.y, 1 )
		elseif(direction == 1)
		then
			this:GetTransform().scale = vec3( math.abs(this:GetTransform().scale.x), this:GetTransform().scale.y, 1 )
		end

	end

	this:GetRigidBody().velocity = vec3(direction * speed, velocity.y, velocity.z)

end -- fn end

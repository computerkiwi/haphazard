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

-- Updates each frame
function Update(dt)
	
	local velocity = this:GetRigidBody().velocity

	--[[local moveSpeed = speed

	if(scared == true)
	then

		moveSpeed = scaredSpeed

	end]]

	cast = Raycast.Cast(this:GetSpaceIndex(), this:GetTransform().position, vec2(0, -1), this:GetCollider().dimensions.y / 1.8, GROUND_LAYER)

	if(velocity.x == 0 or not cast.gameObjectHit:IsValid())
	then
		
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

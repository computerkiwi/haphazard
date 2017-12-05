--[[
FILE: GnomeMovement.lua
PRIMARY AUTHOR: Brett Schiff

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

-- Connections
otherPlayer = nil

ALLY_PROJECTILE_LAYER = 32 --1 << 5

-- Variables
direction = 1 -- direction of movement - -1 for left, 1 for right
speed = 2      -- speed at which boi moves

-- Called at frame start
function Start()
	
end --fn end

-- Updates each frame
function Update(dt)
	
	local velocity = this:GetRigidBody().velocity

	if(velocity.x == 0)
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

-- Other is a game object
function OnCollisionEnter(other)

	if(this:GetDynamicCollider().colliderData:IsCollidingWithLayer(ALLY_PROJECTILE_LAYER))
	then
		this:Destroy()
	end

end -- fn end

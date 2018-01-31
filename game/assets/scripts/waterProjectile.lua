--[[
FILE: waterProjectile.lua
PRIMARY AUTHOR: Max

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

ENEMY_LAYER = 16;

life = 1;

function Start()

	--local dir = GameObject.FindByName("Player2"):GetScript("GnomeMovement.lua").lastDir

	--this:GetRigidBody().velocity = vec3( this:GetRigidBody().velocity.x * dir, this:GetRigidBody().velocity.y, this:GetRigidBody().velocity.z )

end

function Update(dt)

	life = life - dt
	if(life < 0)
	then
		--this:Destroy()
	end

end

function OnCollisionEnter(other)

	if(this:GetDynamicCollider().colliderData:IsCollidingWithLayer(16))
	then
		--other:Destroy()
		this:Destroy()
	end

end

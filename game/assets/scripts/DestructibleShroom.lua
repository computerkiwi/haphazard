--[[
FILE: DestructibleShroom.lua
PRIMARY AUTHOR: Lya Vera

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

DROP = "assets/prefabs/CoinDrop.json"

function Start()
  this:Activate()
end -- fn end

function OnCollisionEnter(other)

  if (other:HastTag("Player"))
  then
  end
  --[[ Colliding with projectile
  if(this:GetDynamicCollider().colliderData:IsCollidingWithLayer(ALLY_PROJECTILE_LAYER))
	then
    SpawnDrop()
	end]]

end -- fn end

function SpawnDrop()

  local drop = GameObject.LoadPrefab(DROP)
  local dropBody = drop:GetRigidBody()
  local velocity = dropBody.velocity

	drop:GetTransform().position = this:GetTransform().position;
  
  velocity.x = XVelocity
  velocity.y = YVelocity

  dropBody.velocity = velocity

end -- fn end
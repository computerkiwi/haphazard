--[[
FILE: DestructibleShroom.lua
PRIMARY AUTHOR: Lya Vera

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

DROP = "assets/prefabs/CoinDrop.json"
ALLY_PROJECTILE_LAYER = 32

YVelocity = 5 -- 1 is up, 0 is nothing, -1 is down
XVelocity = 0 -- 1 is right, 0 is nothing, -1 is left

function Start()
  this:Activate()
end -- fn end

function OnCollisionEnter(other)

  --[[if (other:HasTag("Player"))
  then
    print("Shroom hit!")
    SpawnDrop()
  end]]

  -- Colliding with projectile
  if(other:HasTag("Player"))
	then
    this:Deactivate() -- Kill the coin
    SpawnDrop()
	end

end -- fn end

function SpawnDrop()

  local drop = GameObject.LoadPrefab(DROP)
  local dropBody = drop:GetRigidBody()
  local pos = this:GetTransform().position

  local velocity = dropBody.velocity

  velocity.x = XVelocity
  velocity.y = YVelocity

	drop:GetTransform().position = pos

  dropBody.velocity = velocity

  print("SpawnDrop end")

end -- fn end
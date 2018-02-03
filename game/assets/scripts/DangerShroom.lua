--[[
FILE: DangerShroom.lua
PRIMARY AUTHOR: Noah Sweet

Copyright (c) 2017 DigiPen (USA) Corporation.
]]


SpawnTimerMax = 3.2
SpawnTimer = SpawnTimerMax -- The rate at which spores spawn
YVelocity = 0 -- 1 is up, 0 is nothing, -1 is down
XVelocity = 0 -- 1 is right, 0 is nothing, -1 is left

SPORE = "assets/prefabs/dangerShroom_Spore.json"

function SpawnSpore()

  local spore = GameObject.LoadPrefab(SPORE)
  local sporeBody = spore:GetRigidBody()
  local velocity = sporeBody.velocity

	spore:GetTransform().position = this:GetTransform().position;
  
  velocity.x = XVelocity
  velocity.y = YVelocity

  sporeBody.velocity = velocity
end -- fn end

function Update(dt)
	
	SpawnTimer = SpawnTimer - dt;
	if SpawnTimer <= 0
	then
		SpawnSpore();

		SpawnTimer = SpawnTimerMax;
	end
end -- fn end

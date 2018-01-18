--[[
FILE: DangerShroom.lua
PRIMARY AUTHOR: Noah Sweet

Copyright (c) 2017 DigiPen (USA) Corporation.
]]


SpawnTimerMax = 3.2
SpawnTimer = SpawnTimerMax -- The rate at which spores spawn
SpawnDir = 1 -- Default spawn direction (upwards)

SPORE = "assets/prefabs/dangerShroom_Spore.json"

function SpawnSpore()

  local spore = GameObject.LoadPrefab(SPORE)
  local sporeBody = spore:GetRigidBody()
  local velocity = sporeBody.velocity

	spore:GetTransform().position = this:GetTransform().position;
  
  velocity.x = velocity.x * SpawnDir
  velocity.y = velocity.y * SpawnDir

  sporeBody.velocity = velocity
  
end -- fn end


function Start()

  local rotation = this:GetTransform().rotation

  if (rotation > 0)
  then
   SpawnDir = -1
  end

  print(SpawnDir)

end -- fn end

function Update(dt)
	
	SpawnTimer = SpawnTimer - dt;
	if SpawnTimer <= 0
	then
		SpawnSpore();

		SpawnTimer = SpawnTimerMax;
	end
end -- fn end

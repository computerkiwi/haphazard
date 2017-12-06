--[[
FILE: DangerShroom.lua
PRIMARY AUTHOR: Noah Sweet

Copyright (c) 2017 DigiPen (USA) Corporation.
]]


SpawnTimerMax = 3.2
SpawnTimer = SpawnTimerMax -- The rate at which spores spawn

SPORE = "assets/prefabs/dangerShroom_Spore.json"

function SpawnSpore()
	GameObject.LoadPrefab(SPORE):GetTransform().position = this:GetTransform().position - vec2(0, 0.08f);
end -- fn end


function Start()
end -- fn end

function Update(dt)
	
	SpawnTimer = SpawnTimer - dt;
	if SpawnTimer <= 0
	then
		SpawnSpore();

		SpawnTimer = SpawnTimerMax;
	end
end -- fn end




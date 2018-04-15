--[[
FILE: ProjectileSpawnParticle.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

ParticlePrefab = "ParticleName.json"

SPAWN_OFFSET_X = 0
SPAWN_OFFSET_Y = 0

ParticleObject = nil

function Spawn()
	ParticleObject = GameObject.LoadPrefab("assets\\prefabs\\".. ParticlePrefab)

	local transform = ParticleObject:GetTransform()
	
	ParticleObject:SetName(ParticlePrefab)

	transform.parent = this
	transform.localPosition = vec2(SPAWN_OFFSET_X, SPAWN_OFFSET_Y)

	settings = ParticleObject:GetParticleSystem().settings

	local v = settings.StartingVelocityMinMax
	if(this:GetScript("ProjectileInfo.lua").isRight)
	then
		v.x = math.abs(v.x);
		v.z = math.abs(v.z);
	else
		v.x = -math.abs(v.x);
		v.z = -math.abs(v.z);
	end
	settings.StartingVelocityMinMax = v;

	ParticleObject:GetParticleSystem().settings = settings
end

function Kill()
	ParticleObject:Destroy()
end

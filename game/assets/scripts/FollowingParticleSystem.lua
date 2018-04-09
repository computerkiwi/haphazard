--[[
FILE: FollowingParticleSystem.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]


PARTICLE_OFFSET_X = 0
PARTICLE_OFFSET_Y = 0

ParticlePrefab = "Particles_NAME.json"

ParticleObject = nil
ParticlesEnabled = false

local ParticleRate = 0
local Burst = vec3(0,0,0)

function SetEnabled(shouldBeEnabled)
	-- Don't bother changing if our status is already in the state we want it in.
	if (ParticlesEnabled == shouldBeEnabled or ParticleObject == nil)
	then
		return
	end

	-- Update the status.
	ParticlesEnabled = shouldBeEnabled
	
	local pSystem = ParticleObject:GetParticleSystem()
	local settings = pSystem.settings

	if(ParticlesEnabled)
	then
		settings.EmissionRate = ParticleRate
		settings.BurstEmission = Burst

		if(Burst.x ~= 0)
		then
			pSystem.time = 0
		end
	else
		settings.EmissionRate = 0
		settings.BurstEmission = vec3(0,0,0)
	end

	pSystem.settings = settings
end

function InitParticles()
	local particleObjectName = "__" .. this:GetName() .. "_FOLLOWING_PARTICLES"
	
	-- Delete the particle object if it already exists.
	local existingParticleObject = GameObject.FindByName(particleObjectName)
	if (existingParticleObject:IsValid())
	then
		existingParticleObject:Delete()
	end
	

	-- Create the particle object.
	ParticleObject = GameObject.LoadPrefab(ParticlePrefab)
	ParticleObject:SetName(particleObjectName)
	local transform = ParticleObject:GetTransform()
	
	-- Attach it to the gnome.
	transform.parent = this
	transform.localPosition = vec2(PARTICLE_OFFSET_X, PARTICLE_OFFSET_Y)
	
	-- Initially disable the particle.
	ParticlesEnabled = false
	local pSystem = ParticleObject:GetParticleSystem()
	local settings = pSystem.settings
	-- Save the standard amount of particles per emission and stop emitting particles.
	ParticleRate = settings.EmissionRate
	Burst = settings.BurstEmission
	settings.BurstEmission = vec3(0,0,0)
	settings.EmissionRate = 0
	pSystem.settings = settings
	print("LOAD FOLLOWING PARTICLES SUCCESSFUL")
end
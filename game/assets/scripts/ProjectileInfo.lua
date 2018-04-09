--[[
FILE: ProjectileInfo.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

damage = 1
speed = 10
life = 1
cooldownTime = 0.5
offsetX = 0
offsetY = 0
parented = false
isMelee = false

isRight = true

local lifeTimer = 0
local startTimer = false


function Update(dt)
	if(startTimer)
	then
		lifeTimer = lifeTimer - dt

		if(lifeTimer <= 0)
		then
			if(this:GetScript("ProjectileSpawnParticle.lua") ~= nil)
			then
				this:GetScript("ProjectileSpawnParticle.lua").Kill()
			end
			this:Destroy()
		end
	end
end

function StartLifeTimer()
	startTimer = true
	lifeTimer = life

	if(this:GetScript("ProjectileSpawnParticle.lua") ~= nil)
	then
		this:GetScript("ProjectileSpawnParticle.lua").Spawn()
	end
end

function OnCollisionEnter(other)
	if (not isMelee)
	then
		this:Destroy()
	end
end

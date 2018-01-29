--[[
FILE: ProjectileSpawner.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

-- Prefab settings
PrefabName = "Projectile.json"

-- Spawning settings
relativeFireDirection = true
aimDirection = vec2(1,0)

relativeOffset = true
offset = vec2(0.5, 0)

fireSpeed = 1

parented = false -- TODO. Wait til parenting via script works properly

-- Local variables
local direction = vec2(1,0)	-- Left = -1    Right = 1


function SetDir(dir)
	direction = dir
end

function Fire()
	local proj = GameObject.LoadPrefab(PrefabName)
	
	-- Set offset
	off = offset
	if(relativeOffset)
	then
		off = vec2(offset.x * direction.x, offset.y * direction.y)
	end

	proj.GetTransform().position = proj.GetTransform().position + off

	-- Set Speed
	local dir = aimDirection
	if(relativeFireDirection)
	then
		dir = vec2(direction.x * aimDirection.x, direction.y * aimDirection.y)
	end

	proj.GetRigidBody().velocity = dir * fireSpeed
end

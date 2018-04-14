--[[
FILE: ProjectileSpawner.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

-- Spawning settings
relativeFireDirection = true
aimDirection = vec2(1,0)

relativeOffset = true

fireSpeed = 10

-- Local variables
local direction = vec2(1,0)	-- Left = -1    Right = 1
local timer = 0
local canFire = true

function Update(dt)
	if(canFire == false)
	then
		timer = timer - dt
		if(timer <= 0)
		then
			canFire = true
		end
	end
end

function SetDir(dir)
	direction = dir
end

function SetAim(aim)
	aimDirection = aim
end

function VecLength(vec)
	return math.sqrt(vec.x * vec.x + vec.y * vec.y)
end

function Normalize(vec)
	local len = VecLength(vec)

	vec.x = vec.x / len
	vec.y = vec.y / len

end

-- returns true if we actually fire.
function Fire(PrefabName)
	if (canFire == false) then return false end

	local proj = GameObject.LoadPrefab("assets/prefabs/" .. PrefabName)
	
	local info = proj:GetScript("ProjectileInfo.lua")

	-- Set offset
	off = vec2(info.offsetX, info.offsetY)

	if(relativeOffset)
	then
		off = vec2(off.x * direction.x, off.y)
	end

	proj:GetTransform().position = vec2(this:GetTransform().position.x + off.x, this:GetTransform().position.y + off.y)

	if(direction.x < 0)
	then
		proj:GetTransform().scale = vec3(-proj:GetTransform().scale.x, proj:GetTransform().scale.y, proj:GetTransform().scale.z)
		if (proj:HasStaticCollider() or proj:HasDynamicCollider())
		then
			proj:GetCollider().offset = vec3(-proj:GetCollider().offset.x, proj:GetCollider().offset.y, proj:GetCollider().offset.z)
		end
	end

	proj:GetScript("ProjectileInfo.lua").isRight = direction.x > 0

	-- Set Speed
	local dir = aimDirection
	Normalize(dir)

	proj:GetRigidBody().velocity = vec3(dir.x * info.speed, dir.y * info.speed, 0)

	info.StartLifeTimer()

	canFire = false
	timer = info.cooldownTime

	if(info.parented)
	then
		proj:GetTransform().parent = this
	end
	
	return true
end

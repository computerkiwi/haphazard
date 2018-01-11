--[[
FILE: GreenGnomeAttack.lua
PRIMARY AUTHOR: Ash Duong

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

WATER_PROJECTILE_NAME = "assets/prefabs/waterProjectile.json"

function Update(dt)

	if (IsTriggered(KEY.RightControl))
	then
		local water = GameObject.LoadPrefab(WATER_PROJECTILE_NAME)
		water:GetTransform().position = this:GetTransform().position
	end

	-- Player attacks
	if (GamepadIsPressed(3, 2))
	then
		local water = GameObject.LoadPrefab(WATER_PROJECTILE_NAME)
		water:GetTransform().position = this:GetTransform().position
	end
end

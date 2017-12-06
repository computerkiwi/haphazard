--[[
FILE: BlueGnomeAttack.lua
PRIMARY AUTHOR: Max

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

WATER_PROJECTILE_NAME = "assets/prefabs/waterProjectile.json"

function Update(dt)

	if (IsTriggered(KEY.RightControl))
	then
		local water = GameObject.LoadPrefab(WATER_PROJECTILE_NAME)
		water:GetTransform().position = this:GetTransform().position
	end

end

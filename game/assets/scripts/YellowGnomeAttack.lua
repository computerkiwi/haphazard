--[[
FILE: YellowGnomeAttack.lua
PRIMARY AUTHOR: Ash Duong

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

-- Variables
spawn = false
weapon = nil

-- Defines
WEAPON_NAME = "hammerWeapon.json"

function Start()
  weapon = GameObject.FindByName("YellowGnomeWeapon")
  weapon:Deactivate()
end

function Update(dt)
  if (IsPressed(KEY.Space))
  then
    --Attack()
    --spawn = false
    weapon:Activate()
  else
	if(weapon:IsActive())
	then
	  weapon:Deactivate()
	end
  end

  	-- Player attacks
	if (GamepadIsPressed(2, 2))
	then
		--Attack()
		--spawn = false
		weapon:Activate()
	else
		if(weapon:IsActive())
		then
			weapon:Deactivate()
		end
	end

end -- fn end

function Attack()
  print("Attack!")
--  local weapon = GameObject.LoadPrefab(WEAPON_NAME)
  local weaponTransform = weapon:GetTransform()
  weaponTransform.position = this:GetTransform().position
end -- fn end

function SpawnWeapon()
  spawn = true
end -- fn end
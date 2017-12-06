--[[
FILE: RedGnomeAttack.lua
PRIMARY AUTHOR: Lya Vera

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

-- Variables
spawn = false
weapon = nil

-- Defines
WEAPON_NAME = "hammerWeapon.json"

function Start()
  weapon = GameObject.FindByName("RedGnomeWeapon")
  weapon:Deactivate()
  weapon:GetTransform().parent = this
end

function Update(dt)
  if (IsPressed(89))
  then
    --Attack()
    --spawn = false
    weapon:Activate()
  else
    weapon:Deactivate()
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
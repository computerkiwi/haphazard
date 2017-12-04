--[[
FILE: RedGnomeAttack.lua
PRIMARY AUTHOR: Lya Vera

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

-- Variables
ParentPos = nil

-- Defines
WEAPON = "RedGnomeWeapon.json"

function Update(dt)
  if (ParentPos ~= nil)
  then
    local weapon = GameObject.LoadPrefab(WEAPON)
   local weaponTransform = weapon:GetTransform()
    weaponTransform.position = ParentPos
  end
end -- fn end

function OnCollisionEnter(other)
  if (other:HasTag("Enemy"))
  then
    this:Deactivate()
  end
end -- fn end

function SetParentPos(pos)
  ParentPos = pos
end -- fn end
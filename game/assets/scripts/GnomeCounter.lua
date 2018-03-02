--[[
FILE: GnomeCounter.lua
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

gnomeCount = 0
gnomeCountInternal = 0

-- Recursive function
function CountGnomeStackSize(gnome)
  if (gnome == nil or not gnome:IsValid() or gnome:GetScript("GnomeStatus.lua") == nil)
  then
    return 0
  end
  
  local gnomeStatus = gnome:GetScript("GnomeStatus.lua")
  return (1 + CountGnomeStackSize(gnomeStatus.stackedAbove))
end

function OnCollisionEnter(other)
  -- CountGnomeStackSize returns 0 on nil so it's fine if it's not a gnome.
  gnomeCountInternal = gnomeCountInternal + CountGnomeStackSize(other)
end

function Update(dt)
  gnomeCount = gnomeCountInternal
  
  -- Reset gnomeCountInternal so it can be refreshed with collision events each frame.
  gnomeCountInternal = 0
end

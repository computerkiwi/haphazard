--[[
FILE: DropScript.lua
PRIMARY AUTHOR: Lya Vera

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

DropHeight = 1
Lifetime = 5

function Start()
end -- fn end


function Update(dt)
  Lifetime = Lifetime - dt
  
  -- Timer runs out; destroy
  if (Lifetime <= 0)
  then
    this:Destroy()
--  elseif (this:GetTransform().position.y ~= DropHeight)
    
--  then

  end
end -- fn end


function OnCollisionEnter(object)
  if (not object:HasTag("Enemy") and not object:HasTag("Collectible"))
  then
	  this:Destroy();
  end
end -- fn end
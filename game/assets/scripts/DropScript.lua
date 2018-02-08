--[[
FILE: DropScript.lua
PRIMARY AUTHOR: Lya Vera

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

Lifetime = 15
Invulnerable = 1
Collectible = false
Timer = false

-- Layers
LAYER_DECOR = 1 << 6
LAYER_PLAYER = 1 << 2

function Start()
end -- fn end


function Update(dt)

  Invulnerable = Invulnerable - dt

  if (Invulnerable <= 0)
  then
    StartTimer()
  end

  CheckTimer(dt)

end -- fn end

function StartTimer()
  Timer = true  
  Collectible = true
  local layer = this:GetCollider().collisionLayer
  layer.layer = LAYER_PLAYER
  this:GetCollider().collisionLayer = layer
end -- fn end

function CheckTimer(dt)
  if (Timer == true)
  then
    Lifetime = Lifetime - dt

    -- Timer runs out; destroy
    if (Lifetime <= 0)
    then
      this:Destroy()
    end
  end
end -- fn end

function OnCollisionEnter(object)
  if ((Collectible == true) and (object:HasTag("Player")))
  then
	  this:Destroy();
  end
end -- fn end

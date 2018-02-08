--[[
FILE: DropScript.lua
PRIMARY AUTHOR: Lya Vera

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

Lifetime = 15
Invulnerable = 1
Collectible = false
Timer = false

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

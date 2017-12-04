--[[
FILE: GnomeMovement.lua
PRIMARY AUTHOR: Lya Vera

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

duration = 0.1
player = nil
playerScript = nil

DURATION_RESET = 0.1
xOFFSET = 0.5
yOFFSET = 0.1

function Start()
  player = GameObject.FindByName("Player1")
  playerScript = player:GetScript("GnomeMovement.lua")
end -- fn end

function Update(dt)
  local playerPos = player:GetTransform().position
  local weaponTransform = this:GetTransform()
  local posOffset = playerPos

  local dir = playerScript.GetMoveDir()

  FlipDir(dir)

  -- facing left
  if (dir == -1)
  then
    posOffset.x = playerPos.x - xOFFSET
  -- facing right
  elseif (dir == 1)
  then
    posOffset.x = playerPos.x + xOFFSET
  end

  posOffset.y = playerPos.y - yOFFSET

  weaponTransform.position = posOffset

  --if (duration > 0)
  --then
  --  if (duration <= 0)
  --  then
  --    duration = DURATION_RESET
  --  end
  --  this:Deactivate()
  --end
end -- fn end

function OnCollisionEnter(other)
  if (other:HasTag("Enemy"))
  then
    --TODO: decrement life
  end
end -- fn end

-- Max's flippy code
function FlipDir(dir)
  moveDir = dir

  -- Flip sprite
  if(moveDir == -1)
  then
	  this:GetTransform().scale = vec3( -math.abs(this:GetTransform().scale.x), this:GetTransform().scale.y, 1 )
	--print(this:GetTransform().scale.x)
  elseif(moveDir == 1)
  then
	  this:GetTransform().scale = vec3( math.abs(this:GetTransform().scale.x), this:GetTransform().scale.y, 1 )
	 --print(this:GetTransform().scale.x)
  end
end -- fn end
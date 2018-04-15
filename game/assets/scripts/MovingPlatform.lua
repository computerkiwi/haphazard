--[[
FILE: MovingPlatform.lua
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

vertical = false

speed = 2
moveDistance = 3

currentMoveDir = 1
movedAmount = 0 -- Ranges from 0 to 1

-- Moves horizontally or vertically depending on the flag.
function Move(transform, amount)
  local tempPos = transform.position
  
  if (vertical)
  then
    tempPos.y = tempPos.y + amount
  else
    tempPos.x = tempPos.x + amount
  end
  
  transform.position = tempPos
end

function Update(dt)
  local moveAmount = currentMoveDir * speed * dt
  Move(this:GetTransform(), moveAmount)

  -- Keep track of how much we've moved.
  movedAmount = movedAmount + moveAmount / moveDistance

  -- Check if we need to turn around.
  if (currentMoveDir > 0 and movedAmount > 1)
  then
    currentMoveDir = -1
  elseif(currentMoveDir < 0 and movedAmount < 0)
  then
    currentMoveDir = 1
  end
end

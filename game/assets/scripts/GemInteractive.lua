--[[
FILE: GemInteractive.lua
PRIMARY AUTHOR: Lya Vera

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

startPos = 0
endPos = 0
percent = 0.01
distance = -10 -- Distance to move
steps = 1 -- How fast it moves
moveEnabled = false
moveDir = 0
moveAxis = 0

-- Enums
UP = 1
DOWN = -1
LEFT = -1
RIGHT = 1

function Start()
  -- Make sure object is active
  this:Activate()
  -- Save the start position
  startPos = this:GetTransform().position
  endPos = this:GetTransform().position
  endPos.y = endPos.y + distance
  moveDir = DOWN

end -- fn end

function Update(dt)
  if(moveEnabled)
  then
    print("Attempting to move")

    local transform = this:GetTransform()
    local currPos = transform.position
    local newPos = transform.position

    newPos.y = currPos.y + (percent * (endPos.y - currPos.y))

    transform.position = newPos
    
    if (newPos.y == newPos.y or newPos.y < endPos.y)
    then
      moveEnabled = false
    end
  end

  if (IsPressed(KEY.Enter))
  then
    Reset()
  end
end -- fn end

function OnCollisionEnter(other)
  -- Player collision
  if (other:HasTag("Player"))
  then
    print("Move enabled")
    moveEnabled = true
  end
end -- fn end

-- Public function to be called
function PlaceGem()

end -- fn end

--[[function SetMoveDir(axis, dir)
  
  if (axis == "Horizontal")
  then
    axis = this:GetTransform().position.x
  elseif (axis == "Vertical")
    axis = this:GetTransform().position.y
  end

  if (dir == 1)
  then
    moveDir = 1
  elseif (dir == -1)
    moveDir = -1
  end

end]] -- fn end

-- Reset position
function Reset()
  local transform = this:GetTransform()
  transform.position = startPos
  moveEnabled = false
end --fn end
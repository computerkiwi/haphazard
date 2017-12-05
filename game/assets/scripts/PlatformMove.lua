--[[
FILE: PlatformMove.lua
PRIMARY AUTHOR: Lya Vera

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

-- Variables
moveEnabled = false
moveEnd = false
moveTimer = 2
startPos = nil
vertDir = 1

RESET_TIMER = 2
OFFSET = 0.1 -- y offset to move platform

function Start()
  startPos = this:GetTransform().position
end -- fn end

function Update(dt)
  if (moveEnabled)
  then
    if (moveTimer > 0)
    then
      moveTimer = moveTimer - dt
      MovePlatform()
    else
      moveEnd = true
      moveEnabled = false
      moveTimer = RESET_TIMER
      print("Finished moving")
    end
  end
end -- fn end


function EnablePlatform(dir)
  if (moveEnd == false)
  then
    vertDir = dir
    moveEnabled = true
  end
end -- fn end

function SetTimer(length)
  moveTimer = length
end -- fn end

-- Move the platform
function MovePlatform()
  local transform = this:GetTransform()
  local newPos = transform.position

  if (vertDir > 0)
  then
    newPos.y = newPos.y + OFFSET
  else
    newPos.y = newPos.y - OFFSET
  end

  transform.position = newPos
end -- fn end
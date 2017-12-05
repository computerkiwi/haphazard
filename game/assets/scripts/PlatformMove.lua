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
moveDir = 1

endPos = 1
startPos = 0
delta = 0

RESET_TIMER = 2
SPEED = 1 -- y offset to move platform

function Start()
  startPos = this:GetTransform().position
end -- fn end

function Update(dt)
  if (moveEnabled)
  then
    if (delta < endPos)
    then
      moveTimer = moveTimer - dt
      MovePlatform(dt)
    else
      moveEnd = true
      moveEnabled = false
      moveTimer = RESET_TIMER
      print("Finished moving")
    end
  end
end -- fn end

function EnablePlatform(dir, endPoint)
  if (moveEnd == false)
  then
    moveDir = dir
	endPos = endPoint
	if(dir == 1 or dir == -1)
	then
	  startPos = this:GetTransform().position.y
	else
      startPos = this:GetTransform().position.x
	end
	
    moveEnabled = true
  end
end -- fn end

function SetTimer(length)
  moveTimer = length
end -- fn end

-- Move the platform
function MovePlatform(dt)
  local transform = this:GetTransform()
  local newPos = transform.position

  delta = delta + SPEED *dt
  
  if (moveDir == 1)
  then
    newPos.y = newPos.y + SPEED * dt
  elseif (moveDir == -1)
  then
    newPos.y = newPos.y - SPEED * dt
  elseif (moveDir == 2)
  then
    newPos.x = newPos.x + SPEED * dt
  elseif (moveDir == -2)
  then
	newPos.x = newPos.x - SPEED * dt
  end
	
  transform.position = newPos
end -- fn end
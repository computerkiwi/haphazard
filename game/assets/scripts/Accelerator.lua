--[[
FILE: EnemyBackAndForthMovement.lua
PRIMARY AUTHOR: Brett Schiff

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

directionDegrees = 90
speed = 5

function Start()
	
end

function Update(dt)
	

end

function OnCollisionEnter(other)

  local speedMod = .6
  local trueSpeed = speed * speedMod

  local direction = vec2(math.cos(math.rad(directionDegrees)), math.sin(math.rad(directionDegrees)))

  local tempVelocity = other:GetRigidBody().velocity
  tempVelocity.x = math.lerp((direction.x * trueSpeed), tempVelocity.x, .3)
  tempVelocity.y = math.lerp((direction.y * trueSpeed), tempVelocity.y, .3)

  other:GetRigidBody().velocity = tempVelocity

  if(other:GetScript("GnomeStatus.lua") ~= nil)
  then
	other:GetScript("GnomeStatus.lua").knockedBack = false
  end

end

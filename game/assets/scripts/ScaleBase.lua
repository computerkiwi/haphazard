--[[
FILE: ScaleBase.lua
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

pulleyLeft = nil
pulleyRight = nil

platformLeft = nil
platformRight = nil

gnomeCounterObjLeft = nil
gnomeCounterObjRight = nil

gnomeCounterLeft = nil
gnomeCounterRight = nil

pulleyStartOffsetLeft_x = -3
pulleyStartOffsetLeft_y = 0
pulleyStartOffsetRight_x = 3
pulleyStartOffsetRight_y = 0

averageRopeLength = 3

startingLeftRopeLength = averageRopeLength
leftRopeLength = startingLeftRopeLength

accelerationPerGnome = 0.1
frictionFactor = 0.8
correctionFactor = 0.3

currentVelocity = 0
maxSpeed = 3

function sign(num)
  if (num > 0)
  then
    return 1
  elseif (num < 0)
  then
    return -1
  else
    return 0
  end
end


function SpawnAndAttachObject(prefabName, parentObj)
	local obj = GameObject.LoadPrefab(prefabName)
	if (obj:IsValid())
	then
		obj:GetTransform().parent = parentObj
	
		return obj
	else
		return nil
	end
end

function Start()
  -- Spawn all the objects we need.
  pulleyLeft = SpawnAndAttachObject("assets/prefabs/scale_internal/Pulley.json", this)
  pulleyLeft:GetTransform().localPosition = vec2(pulleyStartOffsetLeft_x, pulleyStartOffsetLeft_y)
  platformLeft = SpawnAndAttachObject("assets/prefabs/scale_internal/ScalePlatform.json", pulleyLeft)
  gnomeCounterObjLeft = SpawnAndAttachObject("assets/prefabs/scale_internal/ScalePlatformGnomeCounter.json", platformLeft)
  gnomeCounterObjLeft:GetTransform().localPosition = vec2(0, 0.15)
  gnomeCounterLeft = gnomeCounterObjLeft:GetScript("GnomeCounter.lua")
  
  pulleyRight = SpawnAndAttachObject("assets/prefabs/scale_internal/Pulley.json", this)
  pulleyRight:GetTransform().localPosition = vec2(pulleyStartOffsetRight_x, pulleyStartOffsetRight_y)
  platformRight = SpawnAndAttachObject("assets/prefabs/scale_internal/ScalePlatform.json", pulleyRight)
  gnomeCounterObjRight = SpawnAndAttachObject("assets/prefabs/scale_internal/ScalePlatformGnomeCounter.json", platformRight)
  gnomeCounterObjRight:GetTransform().localPosition = vec2(0, 0.15)
  gnomeCounterRight = gnomeCounterObjRight:GetScript("GnomeCounter.lua")
  
  leftRopeLength = startingLeftRopeLength
  UpdatePlatformPositions()
end

function CalculatePlatforms(dt)
  local diff = gnomeCounterLeft.gnomeCount - gnomeCounterRight.gnomeCount
  
  currentVelocity = currentVelocity + dt * diff * accelerationPerGnome
  
  if (diff == 0)
  then
    currentVelocity = currentVelocity - (currentVelocity - correctionFactor * currentVelocity) * dt
    leftRopeLength = leftRopeLength - (leftRopeLength - math.lerp(averageRopeLength ,leftRopeLength, correctionFactor)) * dt
  end
  
  leftRopeLength = leftRopeLength + dt * currentVelocity
  
end

function UpdatePlatformPositions()
  platformLeft:GetTransform().localPosition = vec2(0, -leftRopeLength)
  platformRight:GetTransform().localPosition = vec2(0, -(averageRopeLength * 2 - leftRopeLength ))
end

function Update(dt)
  CalculatePlatforms(dt)
  UpdatePlatformPositions()
end



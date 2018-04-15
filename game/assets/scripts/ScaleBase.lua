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

ropeLeft = nil
ropeMid = nil
ropeRight = nil

pulleyStartOffsetLeft_x = -3
pulleyStartOffsetLeft_y = 0
pulleyStartOffsetRight_x = 3
pulleyStartOffsetRight_y = 0

averageRopeLength = 3
minRopeLength = 0.5

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

function PositionRope(rope, pos1, pos2)
	-- Calculate the angle
	local diff = {x = pos1.x - pos2.x, y = pos1.y - pos2.y}
	local length = math.sqrt(diff.x * diff.x + diff.y * diff.y)
	local mid = vec2((pos1.x + pos2.x) / 2, (pos1.y + pos2.y) / 2)
	
	local transform = rope:GetTransform()
	
	-- Move to the right position
	transform.position = mid
	
	-- Stretch to touch both objects.
	local tempScale = transform.scale
	tempScale.y = length
	transform.scale = tempScale
	
	-- Rotate along the diff vector.
	local angle = math.deg(math.atan(diff.y, diff.x)) - 90
	transform.rotation = angle
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

	ropeLeft = GameObject.LoadPrefab("assets/prefabs/scale_internal/Rope.json")
	ropeMid = GameObject.LoadPrefab("assets/prefabs/scale_internal/Rope.json")
	ropeRight = GameObject.LoadPrefab("assets/prefabs/scale_internal/Rope.json")

	leftRopeLength = startingLeftRopeLength
	UpdatePlatformPositions()
end

function CalculatePlatforms(dt)
	local diff = gnomeCounterLeft.gnomeCount - gnomeCounterRight.gnomeCount
	-- The amount of gnomes on the gnome 
	local numHeavyGnomes = math.max(gnomeCounterLeft.gnomeCount, gnomeCounterRight.gnomeCount)

	local targetLength = math.lerp(averageRopeLength, minRopeLength, -sign(diff) * numHeavyGnomes / 4)
	local targetVel = maxSpeed * sign(targetLength - leftRopeLength)
	if (math.abs(targetLength - leftRopeLength) < 0.1)
	then
		targetVel = 0
	end

	currentVelocity = math.lerp(currentVelocity, targetVel, 0.05)
	leftRopeLength = leftRopeLength + dt * currentVelocity

	-- Stop the platforms at the ends.
	if (leftRopeLength < minRopeLength)
	then
		leftRopeLength = minRopeLength
		currentVelocity = math.max(0, currentVelocity)
	elseif( leftRopeLength > averageRopeLength * 2 - minRopeLength)
	then
		leftRopeLength = averageRopeLength * 2 - minRopeLength
		currentVelocity = math.min(0, currentVelocity)
	end
end

function UpdatePlatformPositions()

	-- Move the platforms to the right spots.
	platformLeft:GetTransform().localPosition = vec2(0, -leftRopeLength)
	platformRight:GetTransform().localPosition = vec2(0, -(averageRopeLength * 2 - leftRopeLength ))
	
	-- Update the ropes.
	PositionRope(ropeLeft, pulleyLeft:GetTransform().position, platformLeft:GetTransform().position)
	PositionRope(ropeRight, pulleyRight:GetTransform().position, platformRight:GetTransform().position)
	PositionRope(ropeMid, pulleyLeft:GetTransform().position, pulleyRight:GetTransform().position)
	
end

function SavePulleyOffsets()
  local leftPos = pulleyLeft:GetTransform().localPosition
  local rightPos = pulleyRight:GetTransform().localPosition
  
  pulleyStartOffsetLeft_x = leftPos.x
  pulleyStartOffsetLeft_y = leftPos.y
  
  pulleyStartOffsetRight_x = rightPos.x
  pulleyStartOffsetRight_y = rightPos.y
end

function Update(dt)
  SavePulleyOffsets()
  CalculatePlatforms(dt)
  UpdatePlatformPositions()
end



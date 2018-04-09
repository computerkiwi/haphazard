--[[
FILE: GenericUI.lua
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

offset_x = 0
offset_y = 0

scale_x = 1
scale_y = 1

SCALE_FACTOR = 5

function RotatedVec(x, y, rotation)
	local cosTheta = math.cos(math.rad(rotation))
	local sinTheta = math.sin(math.rad(rotation))

	return vec2(cosTheta * x - sinTheta * y, sinTheta * x + cosTheta * y)
end

function LateUpdate(dt)

	-- Get the components we need and confirm they exist.
	-- Assumes the camera is on the parent of this object.
	local parentObj = this:GetTransform().parent
	if (not parentObj:IsValid())
	then
		return nil
	end
	
	if (not parentObj:HasCamera())
	then
		return nil
	end
	
	local parentRot = parentObj:GetTransform().rotation
	local cam = parentObj:GetCamera()
	local transform = this:GetTransform()	
	
	-- Turn the camera zoom into a usable value.
	local zoomScale = cam.zoom / SCALE_FACTOR
	
	transform.localPosition = RotatedVec(zoomScale * offset_x, zoomScale * offset_y, parentRot)
	transform.scale = vec3(zoomScale * scale_x, zoomScale * scale_y, 1)
	transform.rotation = parentRot
end

-- Make sure it runs while paused.
function PausedLateUpdate()
	LateUpdate()
end

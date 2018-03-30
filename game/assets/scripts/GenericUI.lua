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
	
	local cam = parentObj:GetCamera()
	local transform = this:GetTransform()	
	
	-- Turn the camera zoom into a usable value.
	local zoomScale = cam.zoom / SCALE_FACTOR
	
	transform.localPosition = vec2(zoomScale * offset_x, zoomScale * offset_y)
	transform.scale = vec3(zoomScale * scale_x, zoomScale * scale_y, 1)
end

-- Make sure it runs while paused.
function PausedLateUpdate()
	LateUpdate()
end

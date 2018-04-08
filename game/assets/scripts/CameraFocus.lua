--[[
FILE: CameraFocus.lua
PRIMARY AUTHOR: Kieran

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

-- If the object is closer than INNER_THRESHOLD units from the unfocused camera, it will be fully onscreen.
-- If the object is closer than OUTER_THRESHOLD units from the unfocused camera, the camera will start to move towards it.
_INNER_THRESHOLD = 12
_OUTER_THRESHOLD = 20

LEFT_DISTANCE_MULTIPLIER = 1
RIGHT_DISTANCE_MULTIPLIER = 1
TOP_DISTANCE_MULTIPLIER = 1
BOT_DISTANCE_MULTIPLIER = 1

enabled = true

function Update(dt)
	if (enabled and _G.focusObjects)
	then
		table.insert(_G.focusObjects, this)
	end
end

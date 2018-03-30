--[[
FILE: SimplePause.lua
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

function MyUpdate()
	if OnPress(KEY.Escape)
	then
		SetPaused(not IsPaused())
	end
end

function Update()
	MyUpdate()
end

function PausedUpdate()
	MyUpdate()
end

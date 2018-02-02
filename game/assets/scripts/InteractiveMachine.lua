--[[
FILE: InteractiveMachine.lua
PRIMARY AUTHOR: Ash Duong

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

Activated = false

function OnCollisionEnter(other)
	if other:HasTag("Gem")
	then
		Activated = true
	end
end
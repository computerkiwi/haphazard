--[[
FILE: ParentToCamera.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

dad = nil

offset = vec2(0,0)

function Start()
	dad = GameObject.FindByName("Main Camera");
	offset = this:GetTransform().position
end

function Update(dt)
	this:GetTransform().position = vec2(dad:GetTransform().position.x + offset.x, dad:GetTransform().position.y + offset.y)
end

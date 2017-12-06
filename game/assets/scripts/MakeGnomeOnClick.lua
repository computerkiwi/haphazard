--[[
FILE: MakeGnomeOnClick.lua
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

function Update(dt)
	if (IsTriggered(0)) --Mouse_1
	then
		local gnome = GameObject.LoadPrefab("gnome.json")
		local transform = gnome:GetTransform()
		
		transform.position = GetMousePos()
		
	end
end
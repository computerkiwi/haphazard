--[[
FILE: EmoteSpawner.lua
PRIMARY AUTHOR: Ash Duong

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

function Start()
	-- Remove an emote object if it exists.
	local nameString = "__Emote_Obj_" .. this:GetName()
	local tempObj = GameObject.FindByName(nameString)
	if (tempObj:IsValid())
	then
		tempObj:Destroy()
	end

	
	local emote = GameObject.LoadPrefab("assets/prefabs/Emotes.json")
	emote:SetName(nameString)
	emote:GetTransform().parent = this
	emote:GetScript("Emotes.lua").Player = this
	emote:GetTransform().localPosition = vec2(0.7, 0.9)
end
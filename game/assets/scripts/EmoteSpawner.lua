--[[
FILE: EmoteSpawner.lua
PRIMARY AUTHOR: Ash Duong

Copyright (c) 2018 DigiPen (USA) Corporation.
]]
  
function Start()
	local p1emotes = GameObject.LoadPrefab("assets/prefabs/Emotes.json")
	p1emotes:GetTransform().parent = GameObject.FindByName("Player1")
	p1emotes:GetScript("Emotes.lua").Player = p1emotes:GetTransform().parent
	local newpos1 = p1emotes:GetTransform().position
	newpos1.x = GameObject.FindByName("Player1"):GetTransform().position.x + 0.5
	newpos1.y = GameObject.FindByName("Player1"):GetTransform().position.y + 0.5
	p1emotes:GetTransform().position = newpos1

	local p2emotes = GameObject.LoadPrefab("assets/prefabs/Emotes.json")
	p2emotes:GetTransform().parent = GameObject.FindByName("Player2")
	p2emotes:GetScript("Emotes.lua").Player = p2emotes:GetTransform().parent
	local newpos2 = p2emotes:GetTransform().position
	newpos2.x = GameObject.FindByName("Player2"):GetTransform().position.x + 0.5
	newpos2.y = GameObject.FindByName("Player2"):GetTransform().position.y + 0.5
	p2emotes:GetTransform().position = newpos2

	local p3emotes = GameObject.LoadPrefab("assets/prefabs/Emotes.json")
	p3emotes:GetTransform().parent = GameObject.FindByName("Player3")
	p3emotes:GetScript("Emotes.lua").Player = p3emotes:GetTransform().parent
	local newpos3 = p3emotes:GetTransform().position
	newpos3.x = GameObject.FindByName("Player3"):GetTransform().position.x + 0.5
	newpos3.y = GameObject.FindByName("Player3"):GetTransform().position.y + 0.5
	p3emotes:GetTransform().position = newpos3

	local p4emotes = GameObject.LoadPrefab("assets/prefabs/Emotes.json")
	p4emotes:GetTransform().parent = GameObject.FindByName("Player4")
	p4emotes:GetScript("Emotes.lua").Player = p4emotes:GetTransform().parent
	local newpos4 = p4emotes:GetTransform().position
	newpos4.x = GameObject.FindByName("Player4"):GetTransform().position.x + 0.5
	newpos4.y = GameObject.FindByName("Player4"):GetTransform().position.y + 0.5
	p4emotes:GetTransform().position = newpos4
end
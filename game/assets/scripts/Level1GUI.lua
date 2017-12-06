--[[
FILE: Level1GUI.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

cam = nil

offset = vec2(0,0)
startScale = vec3(0,0,0)

function Start()
	cam = GameObject.FindByName("Main Camera");

	if(this:GetName() == "Player1Healthbar")
	then
		offset = vec2(-2,2)
	elseif(this:GetName() == "Player2Healthbar")
	then
		offset = vec2(2,2)
	elseif(this:GetName() == "GemCounter")
	then
		offset = vec2(0,2)
	end

	startScale = this:GetTransform().scale
	--offset = this:GetTransform().position

end

function Update(dt)
	local zoomScale = cam:GetCamera().zoom / 5
	this:GetTransform().position = vec2(cam:GetTransform().position.x + offset.x * zoomScale, cam:GetTransform().position.y + offset.y * zoomScale)
	this:GetTransform().scale = vec3(startScale.x * zoomScale, startScale.y * zoomScale, 1)
end

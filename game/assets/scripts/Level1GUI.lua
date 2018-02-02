--[[
FILE: Level1GUI.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

cam = nil

offset = vec2(0,0)
startScale = vec3(0,0,0)

XOffset = 0
YOffset = 0

function Start()
	cam = GameObject.FindByName("Main Camera");
  --[[ Changed to specific scale to avoid UI getting larger whenever
       I zoom out, edit a level, save, and load. -- Lya ]]
  if(this:GetName() == "GemCounter")
  then
    startScale.x = 1.5
    startScale.y = 0.5
  else
    startScale.x = 1
    startScale.y = 1
  end

	--offset = this:GetTransform().position

end

function Update(dt)

	local zoomScale = cam:GetCamera().zoom / 5
	this:GetTransform().position = vec2(cam:GetTransform().position.x + XOffset * zoomScale, cam:GetTransform().position.y + YOffset * zoomScale)
	this:GetTransform().scale = vec3(startScale.x * zoomScale, startScale.y * zoomScale, 1)
end

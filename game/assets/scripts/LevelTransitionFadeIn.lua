--[[
FILE: LevelTransitionFadeIn.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

transitionObj = nil
killTime = 2.9
ended = false

function Start()
	transitionObj = GameObject.LoadPrefab("assets/prefabs/level_transition/LevelTransitionIn.json")
	transitionObj:GetTransform().position = GameObject.FindByName("Main Camera"):GetTransform().position
	ended = false
  killTime = 2.9
end

function Update(dt)
	if(ended == false)
	then
		killTime = killTime - dt

		transitionObj:GetTransform().position = GameObject.FindByName("Main Camera"):GetTransform().position

		if(killTime < 0)
		then
			transitionObj:Destroy()
			ended = true
		end
	end
end

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
	transitionObj:GetTransform().position = ScreenToWorld(vec2(0,0))
	ended = false
end

function Update(dt)
	if(ended == false)
	then
		killTime = killTime - dt

		transitionObj:GetTransform().position = ScreenToWorld(vec2(0,0))

		if(killTime < 0)
		then
			transitionObj:Destroy()
			ended = true
		end
	end
end
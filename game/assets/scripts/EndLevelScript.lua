--[[
FILE: EndLevelScript.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

WIN_LEVEL = "YouWinLevel.json"

transitionTime = 5

transitionStarted = false

transitionScreen = nil
transitionSparkles = nil

nextLevelKey = KEY.N

function Start()
  -- Initialize the end screen table if it doesn't exist yet.
  _G.globalEndScreenTable = _G.globalEndScreenTable or {nextLevel = "defaultLevel.json"}
end

function GoToNextLevel()

	_G.globalEndScreenTable.nextLevel = WIN_LEVEL
	Engine.LoadLevel("LevelEndScreen.json")

end

function StartTransition()

	transitionStarted = true

	transitionScreen = GameObject.LoadPrefab("assets/prefabs/level_transition/LevelTransition.json")
	transitionSparkles = GameObject.LoadPrefab("assets/prefabs/level_transition/LevelTransitionSparkles.json")
	local winSparkles = GameObject.LoadPrefab("assets/prefabs/level_transition/LevelTransitionWin.json")

	transitionScreen:GetTransform().position = ScreenToWorld(vec2(0,0))
	transitionSparkles:GetTransform().position = ScreenToWorld(vec2(0,0))
	
	winSparkles:GetTransform().position = this:GetTransform().position
	winSparkles:GetTransform().position = vec2(winSparkles:GetTransform().position.x, winSparkles:GetTransform().position.y - 1);

end

function Update(dt)
	if(transitionStarted)
	then
		transitionTime = transitionTime - dt

		transitionScreen:GetTransform().position = ScreenToWorld(vec2(0,0))
		transitionSparkles:GetTransform().position = ScreenToWorld(vec2(0,0))

		if(transitionTime < 0)
		then
			GoToNextLevel()
		end

	end

	if(_G.CHEAT_MODE and IsHeld(KEY.LeftControl) and OnPress(nextLevelKey))
	then
		
		StartTransition()

	end
end

function OnCollisionEnter(other)
  -- Player takes gem
  if (other:HasTag("Player") and transitionStarted == false)
  then
	StartTransition()
  end
  
end

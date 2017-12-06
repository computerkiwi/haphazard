--[[
FILE: PlayerStats.lua
PRIMARY AUTHOR: Lya Vera

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

-- Variables
Player1_Health = 10
Player2_Health = 10
TotalLives = 5
TotalScore = 0
CoinValue = 1
GemsCollected = 0

function Update(dt)
  if (TotalScore == 5)
  then
--    local platform = GameObject.FindByName("InteractiveWall")
--    local script = platform:GetScript("PlatformMove.lua")
--    script.EnablePlatform()
  end
end -- fn end

function AddLife()
  TotalLives = TotalLives + 1
end -- fn end

function LoseLife()
  TotalLives = TotalLives - 1
end -- fn end

function AddScore()
  TotalScore = TotalScore + CoinValue

  print("Score: ")
  print(TotalScore)
end -- fn end

function AddGem()
	GemsCollected = GemsCollected + 1

	local tex = GameObject.FindByName("GemCounter"):GetSprite().textureHandler
	tex.currentFrame = GemsCollected
	GameObject.FindByName("GemCounter"):GetSprite().textureHandler = tex
end

function GetGems()
	return GemsCollected
end

function ResetScore()

  TotalScore = 0
end -- fn end
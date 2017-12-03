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

function AddLife()
  TotalLives = TotalLives + 1
end -- fn end

function LoseLife()
  TotalLives = TotalLives - 1
end -- fn end

function AddScore()
  TotalScore = TotalScore + CoinValue
end -- fn end

function ResetScore()

  TotalScore = 0
end -- fn end
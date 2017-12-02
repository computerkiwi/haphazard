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

function PlayerDamage()
end -- fn end

function AddScore()

  TotalScore = TotalScore + CoinValue

  print("Coin value:")
  print(CoinValue)

end -- fn end
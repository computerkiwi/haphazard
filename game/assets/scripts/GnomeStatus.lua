--[[
FILE: GnomeStatus.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2018 DigiPen (USA) Corporation.
]]


-- Player info

PLAYER_NUM = 0
PLAYER_PHYS_LAYER = 1 << 2

-- Gnome type

GnomeType = 1
--[[
Types:
Red	   = 1
Green  = 2
Blue   = 3
Yellow = 4
]]

-- Stacked info

stacked = false
stackedAbove = nil
stackedBelow = nil

-- Movement info

canMove = true
knockedBack = false

specialMove = false
specialMoveScale = 0.5

specialJump = false
specialJumpScale = 2

-- Health info
isStatue = false

-- Collectibles
hasGem = false

-- Apply the current gnome type. (Change sprite, etc.)
function SetInfo(playerNum, gnomeId)
	
	PLAYER_NUM = playerNum
	GnomeType = gnomeId

	this:SetName("Player" .. tostring(playerNum))

	PLAYER_PHYS_LAYER = 1 << (6 + playerNum)

	local abilityScript = this:GetScript("GnomeAbilities.lua")
	if (abilityScript ~= nil)
	then
		abilityScript.SetType(gnomeId)
	end

end


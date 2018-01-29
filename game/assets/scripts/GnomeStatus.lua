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

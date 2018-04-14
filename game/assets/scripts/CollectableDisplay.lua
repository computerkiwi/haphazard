--[[
FILE: CollectableDisplay.lua
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

-- Make sure the global collectable table is initialized
do
	local function DefaultCollectFunc()
		_G.collectablesInfo.collected = _G.collectablesInfo.collected + 1
	end

	if (_G.collectablesInfo == nil)
	then
		_G.collectablesInfo = {}
		local ci = _G.collectablesInfo
		ci.collected = 0
		ci.Collect = DefaultCollectFunc
	end
end

-- How many collected this level.
local collected = 0

local function ThisCollect()
	collected = collected + 1
	_G.collectablesInfo.collected = _G.collectablesInfo.collected + 1
end

function Start()
	-- Register our collect function.
	_G.collectablesInfo.Collect = ThisCollect
end

function Update(dt)
	local spr = this:GetSprite()
	local th = spr.textureHandler
	th.currentFrame = collected
	spr.textureHandler = th
end

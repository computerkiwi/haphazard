--[[
FILE: ChallengeScript.lua
PRIMARY AUTHOR: Lya Vera

Copyright (c) 2017 DigiPen (USA) Corporation.
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

function Start()
  this:Activate()
end -- fn end

function OnCollisionEnter(other)
  -- Player takes coin
  if (other:HasTag("Player"))
  then
		_G.collectablesInfo.Collect()
		
		-- TODO: Do sounds/effects here.

    -- Deactivate the object
    this:Deactivate()
  end
end -- fn end

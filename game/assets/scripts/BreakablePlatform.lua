--[[
FILE: BreakablePlatform.lua
PRIMARY AUTHOR: Brett Schiff

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

COLLISION_LAYER_NO_COLLSION = 0
COLLISION_LAYER_GROUND = 1 << 3 -- 8

numGnomesToBreak = 2
timeToReformSeconds = 2
normalCollisionLayer = COLLISION_LAYER_GROUND

Sprite_Platform = "breakablePlatform.json"

isBroken = false;
currTimer = 0;

function Start()

	this:GetSprite().id = Resource.FilenameToID(Sprite_Platform)

end

function Update(dt)

	-- if the platform is not broken, make sure there aren't enough gnomes to break it on top
	if(not isBroken)
	then

		local gnomeCount = this:GetScript("GnomeCounter.lua").gnomeCount

		if(gnomeCount >= numGnomesToBreak)
		then
			
			isBroken = true;

			this:GetCollider().collisionLayer = CollisionLayer(COLLISION_LAYER_NO_COLLSION)

		end

	else

		if(currTimer <= timeToReformSeconds)
		then

			currTimer = currTimer + dt

		else

			currTimer = 0
			isBroken = false
			this:GetCollider().collisionLayer = CollisionLayer(normalCollisionLayer)

		end

	end

end

function OnCollisionEnter(other)

	

end
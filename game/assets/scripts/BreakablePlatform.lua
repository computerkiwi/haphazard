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

particleObject = "assets/prefabs/Particles_BreakEffect.json"

gnomeCounterObj = nil
gnomeCounter = nil

Sprite_Platform = "PlatformBreaking.json"

isBroken = false;
currTimer = 0;

function SpawnAndAttachObject(prefabName, parentObj)
	local obj = GameObject.LoadPrefab(prefabName)
	if (obj:IsValid())
	then
		obj:GetTransform().parent = parentObj
	
		return obj
	else
		return nil
	end
end

function SpawnGnomeCounter()

	gnomeCounterObj = SpawnAndAttachObject("assets/prefabs/breakable_internal/BreakablePlatformGnomeCounter.json", this)
	gnomeCounterObj:GetTransform().localPosition = vec2(0, 0.25)

	local tempScale = gnomeCounterObj:GetTransform().scale
	tempScale.x = this:GetTransform().scale.x
	gnomeCounterObj:GetTransform().scale = tempScale
	gnomeCounterObj:GetCollider().dimensions = tempScale
	
	gnomeCounter = gnomeCounterObj:GetScript("GnomeCounter.lua")

end

function SetSolidAnimation()

	local tex = this:GetSprite().textureHandler
	tex.currentFrame = 0
	this:GetSprite().textureHandler = tex

end

function SetCrackedAnimation()

	local tex = this:GetSprite().textureHandler
	tex.currentFrame = 1
	this:GetSprite().textureHandler = tex

end

function SetBrokenAnimation()

	local tex = this:GetSprite().textureHandler
	tex.currentFrame = 2
	this:GetSprite().textureHandler = tex

end


function Start()

	this:GetSprite().id = Resource.FilenameToID(Sprite_Platform)
	SpawnGnomeCounter()

end

function Update(dt)

	-- if the platform is not broken, make sure there aren't enough gnomes to break it on top
	if(not isBroken)
	then

		local gnomeCount = gnomeCounter.gnomeCount


		if(gnomeCount == 0)
		then

			SetSolidAnimation()

		elseif(gnomeCount >= numGnomesToBreak)
		then
		
			isBroken = true;
			SetBrokenAnimation()

			local particles = SpawnAndAttachObject(particleObject, this)
			particles:GetTransform().position = this:GetTransform().position

			this:GetCollider().collisionLayer = CollisionLayer(COLLISION_LAYER_NO_COLLSION)

		elseif(gnomeCount > 0)
		then
			
			SetCrackedAnimation()

		end

	else

		if(currTimer <= timeToReformSeconds)
		then

			currTimer = currTimer + dt

		else

			currTimer = 0
			isBroken = false
			SetSolidAnimation()
			this:GetCollider().collisionLayer = CollisionLayer(normalCollisionLayer)

		end

	end

end

function OnCollisionEnter(other)

	

end
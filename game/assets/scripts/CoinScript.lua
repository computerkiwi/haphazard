--[[
FILE: CoinScript.lua
PRIMARY AUTHOR: Lya Vera

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

CoinLayer = 1 << 12

local PARTICLE_PREFAB_OBJECT = "assets/prefabs/CoinEffect.json"

function Start()
  this:Activate()

  this:GetCollider().collisionLayer = CollisionLayer(CoinLayer)

  local tex = this:GetSprite().textureHandler
  tex.fps = 0
  tex.currentFrame = math.random(0, 4)
  this:GetSprite().textureHandler = tex
end -- fn end

function OnCollisionEnter(other)
  -- Player takes coin
  if (other:HasTag("Player"))
  then
    PlaySound("Coin6.wav", 1, 1, false)

	local particles = GameObject.LoadPrefab(PARTICLE_PREFAB_OBJECT)
	particles:GetTransform().position = this:GetTransform().position

    -- Deactivate the object
    this:Deactivate()
  end
end -- fn end
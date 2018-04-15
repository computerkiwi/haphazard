--[[
FILE: DropScript.lua
PRIMARY AUTHOR: Lya Vera

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

Lifetime = 15
Invulnerable = 0.5
Collectible = false
Timer = false

-- Layers
LAYER_DECOR = 1 << 6
LAYER_PLAYER = 1 << 2

local startY

local PARTICLE_PREFAB_OBJECT = "assets/prefabs/CoinEffect.json"

function Start()
end -- fn end


function Update(dt)

  Invulnerable = Invulnerable - dt

  if (Invulnerable <= 0)
  then
    StartTimer()
  end

  CheckTimer(dt)
  SimulateGround()
  
end -- fn end

function Start()
  startY = this:GetTransform().position.y
end 

-- Passthrough collision means it feels good to collect but doesn't touch the ground.
-- Solution: Simulate our own ground. (Assumption: flowers always placed on the ground.)
function SimulateGround()
  local transform = this:GetTransform()
  local pos = transform.position
  
  if (pos.y < startY)
  then
    pos.y = startY
    transform.position = pos
    
    this:GetRigidBody().velocity = vec3(0, 0, 0)
    this:GetRigidBody().gravity = vec3(0, 0, 0)
  end
end

function StartTimer()
  Timer = true  
  Collectible = true
  local layer = this:GetCollider().collisionLayer
  layer.layer = LAYER_PLAYER
  this:GetCollider().collisionLayer = layer
end -- fn end

function CheckTimer(dt)
  if (Timer == true)
  then
    Lifetime = Lifetime - dt

    -- Timer runs out; destroy
    if (Lifetime <= 0)
    then
      this:Destroy()
    end
  end
end -- fn end

function OnCollisionEnter(object)
  if ((Collectible == true) and (object:HasTag("Player")))
  then
    PlaySound("Coin6.wav", 0.2, 1, false)
	local particles = GameObject.LoadPrefab(PARTICLE_PREFAB_OBJECT)
	particles:GetTransform().position = this:GetTransform().position
    this:Destroy();
  end
end -- fn end

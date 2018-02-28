--[[
FILE: EnemySpawner.lua
PRIMARY AUTHOR: Lya Vera

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

ENEMY = "assets/prefabs/Creepyboi.json"

Enabled = true -- Enabled the spawner
SpawnTimerMax = 3.2
SpawnTimer = SpawnTimerMax
SpawnTotalMax = 5  -- Total number of enemies to spawn
SpawnTotal = SpawnTotalMax
SpawnKey = KEY.Numpad_9

function Start()
  this:Active()
end -- fn end

function Update(dt)
  if (IsHeld(SpawnKey))
  then
    Enabled = Enabled or true
    print(Enabled)
  end

  if (Enabled)
  then
    SpawnTimer = SpawnTimer - dt
    if (SpawnTimer <= 0)
    then
      SpawnEnemy()
      SpawnTimer = SpawnTimerMax
    end
  end
end -- fn end

-- Disable the spawner
function DisableSpawner()
  Enabled = false
end -- fn end

-- Enable the spawner
function EnableSpawner()
  Enabled = true
end -- fn end

function SpawnEnemy()
  -- Spawner is enabled and can spawn enemies
  if (SpawnTotal > 0)
  then
    local enemy = GameObject.LoadPrefab(ENEMY)
    SpawnTotal = SpawnTotal - 1

    enemy:GetTransform().position = this:GetTransform().position

  -- Disable timer when all enemies have been spawned
  elseif (SpawnTotal == 0)
  then
    Enabled = false
    SpawnTotal = SpawnTotalMax
  end
end -- fn end
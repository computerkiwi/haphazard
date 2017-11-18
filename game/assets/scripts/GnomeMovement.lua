--[[
FILE: GnomeMovement.lua
PRIMARY AUTHOR: Lya Vera

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

-- Variables
moveSpeed  = 5.0
gravity    = 2
jumpHeight = 2
moveDir    = 0

-- Bools
jumpEnabled  = false
moveEnabled  = true
stackEnabled = false

-- Enums
PLAYER_LAYER  =  4

MOVEDIR_LEFT  = -1 -- Player moving left
MOVEDIR_IDLE  =  0 -- Player idle
MOVEDIR_RIGHT =  1 -- Player moving right

MOVE_RIGHT = 85 -- U

-- Updates each frame
function Update(dt)
  
  -- Determine player
  local name = this:GetName()

  if (name == "Player1")
  then
    PLAYER_NUM = 0

  else
    PLAYER_NUM = 1
  
  end

  UpdateMovement(dt)
end


function UpdateMovement(dt)
  local playerBody = this:GetRigidBody()
  --local playerTransform = this:GetTransform()
  
  if (playerBody != nil)
    print("Body success!")
  end

  --newVelocity = playerBody.velocity

  -- Player moving right
  --if (IsPressed(MOVE_RIGHT))
  --  newVelocity.x = moveSpeed
  --end

  -- Update player velocity
  --playerBody.velocity = newVelocity
end

-- Other is a game object
function OnCollisionEnter(other)
  
  print("Collision enter")

end
--[[
FILE: TossScript.lua
PRIMARY AUTHOR: Lya Vera

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

tossEnabled = false
throwSpeed = 10
throwForce = 15


function TossPlayer(moveDir)
  -- Connections
  local playerBody = this:GetRigidBody()
  local playerTransform = this:GetTransform()

  local newVelocity = playerBody.velocity
  local acceleration = playerBody.acceleration

  -- Calculate x velocity
  newVelocity.x = moveDir * throwSpeed
  newVelocity.y = throwForce

  -- Update player velocity
  playerBody.velocity = newVelocity
end -- fn end

function EnableToss()

tossEnabled = true

end -- fn end


function DisableToss()

tossEnabled = false

end -- fn end
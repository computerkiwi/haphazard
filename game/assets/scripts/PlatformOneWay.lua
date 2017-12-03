--[[
FILE: OnewayPlatform.lua
PRIMARY AUTHOR: Lya Vera

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

-- Variables
openRot = 90
closeRot = -90
platformOpen = false

-- On awake, set platformOpen to false

function Update(dt)

  -- Platform is open
  if (platformOpen == true)
  then
    
    -- Better logic here
    local transform = this:GetTransform()
    transform.rotation = closeRot
    platformOpen = false
  end
end -- fn end

function OncCollisionEnter(other)

  local otherTag = other:HasTag()

  -- Player collides with platform
  if (other:HasTag("Player"))
  then
    
    platformOpen = true

    -- Rotate platform
    local transform = this:GetTransform()
    transform.rotation = openRot
  end
end -- fn end


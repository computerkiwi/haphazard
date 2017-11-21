--[[
FILE: GnomeMovement.lua
PRIMARY AUTHOR: Lya Vera

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

-- Variables
moveSpeed  = 5.0
gravity    = 2
jumpHeight = 2

-- Bools
jumpEnabled  = false
moveEnabled  = true
stackEnabled = false

-- Enums
PLAYER_LAYER  =  4

PLAYER_NUM = -1 -- Set temporarily

-- Move directions
LEFT  = -1 -- Player moving left
IDLE  =  0 -- Player idle
RIGHT =  1 -- Player moving right

DEADZONE = 0.05 -- Joystick dead zone

-- Gamepad Buttons found under KeyMap.h
JUMP   = 0 -- A
ATTACK = 1 -- B

HORIZONTAL_AXIS = 0

function UpdateMovement(moveDir)
  local playerBody = this:GetRigidBody()
  local playerTransform = this:GetTransform()

  --newVelocity = playerBody.velocity

  local dir = moveDir

  newVelocity.x = dir * moveSpeed

  -- Update player velocity
  playerBody.velocity = newVelocity
end

-- Updates each frame
function Update(dt)
  
  -- Can we add playerID's to the player objects? :<

  -- TEMP move to start function
  -- Determine player
  if (PLAYER_NUM < 0)
  then
    local name = this:GetName()
    SetKeyboardControls(name)
  end

  local moveDir

  -- Retrieve input
  if (GamepadsConnected() > 0)
  then
    print("GamepadsConnected")
    moveDir = GetInputGamepad()
  else
    print("No gamepads")
    moveDir = GetInputKeyboard()
  end

  --UpdateMovement(moveDir)
end

-- Other is a game object
function OnCollisionEnter(other)


end

-- TEMP
function SetKeyboardControls(name)

  if (name == "Player1")
  then
    PLAYER_NUM = 0

    print("Set to player 1")

    -- TEMPORARY
    KEY_JUMP  = 87 -- W
    KEY_DOWN  = 83 -- S
    KEY_LEFT  = 65 -- A
    KEY_RIGHT = 68 -- D

  else
    PLAYER_NUM = 1

    -- TEMPORARY
		KEY_JUMP  = 265 -- Up
		KEY_DOWN  = 264 -- Down
		KEY_LEFT  = 263 -- Left
		KEY_RIGHT = 262 -- Right

  end

end

-- Gamepad input
function GetInputGamepad()

  local moveDir
  
  if (GamepadGetAxis(PLAYER_NUM, HORIZONTAL_AXIS) > DEADZONE)
  then
    moveDir = MOVEDIR_RIGHT
  elseif (GamepadGetAxis(PLAYER_NUM, HORIZONTAL_AXIS) < -DEADZONE)
  then
    moveDir = MOVEDIR_LEFT
  else
    moveDir = MOVEDIR_IDLE
  end

  return moveDir
end

-- Keyboard input
function GetInputKeyboard()
  
  local moveDir

  if (IsPressed(KEY_RIGHT))
  then
    moveDir = MOVE_RIGHT
  elseif (IsPressed(KEY_LEFT))
  then
    moveDir = MOVE_LEFT
  else
    moveDir = MOVE_IDLE
  end

  --

  local playerBody = this:GetRigidBody()
  local playerTransform = this:GetTransform()

  newVelocity = playerBody.velocity

  newVelocity.x = moveDir * moveSpeed

  -- Update player velocity
  playerBody.velocity = newVelocity

  --
end
--[[
FILE: GnomeMovement.lua
PRIMARY AUTHOR: Lya Vera

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

-- Variables
moveSpeed  = 5.0
jumpHeight = 2
moveDir    = 0
gravity    = 0.5

-- Bools
jumpEnabled  = false
moveEnabled  = true
stackEnabled = false
grounded     = true

-- Enums
PLAYER_LAYER  =  4

PLAYER_NUM = -1 -- Set temporarily

-- Move directions
MOVE_LEFT  = -1 -- Player moving left
MOVE_IDLE  =  0 -- Player idle
MOVE_RIGHT =  1 -- Player moving right

DEADZONE = 0.05 -- Joystick dead zone

-- Gamepad Buttons found under KeyMap.h
JUMP   = 0 -- A
ATTACK = 1 -- B

HORIZONTAL_AXIS = 0

function UpdateMovement(dt)

  -- Connections
  local playerBody = this:GetRigidBody()
  local playerTransform = this:GetTransform()

  newVelocity = playerBody.velocity

  -- Calculate x velocity
  newVelocity.x = moveDir * moveSpeed

  -- Calculate y valocity
  if (jumpEnabled == true and grounded == true)
  then
    newVelocity.y = jumpHeight
    jumpEnabled = false
    grounded = false
  elseif (newVelocity.y < 0)
  then 
    newVelocity.y = newVelocity.y * gravity
  end

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

  -- Retrieve input
  if (GamepadsConnected() > 0)
  then
    print("Using gamepads")
    GetInputGamepad()
  else
    print("Using keyboard")
    GetInputKeyboard()
  end

  -- Update player movement
  UpdateMovement(dt)

end

-- Other is a game object
function OnCollisionEnter(other)
  -- Connections
  local otherName = other:GetName()

  -- Player is on the ground
  if (otherName == "Ground")
  then
    grounded = true
  end

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

  -- Player moves right
  if (GamepadGetAxis(PLAYER_NUM, HORIZONTAL_AXIS) > DEADZONE)
  then
    moveDir = MOVE_RIGHT
  -- Player moves left
  elseif (GamepadGetAxis(PLAYER_NUM, HORIZONTAL_AXIS) < -DEADZONE)
  then
    moveDir = MOVE_LEFT
  -- Player does not move
  else
    moveDir = MOVE_IDLE
  end

  -- Player jumps
--  if (GamepadIsPressed(PLAYER_NUM, x))

end

-- Keyboard input
function GetInputKeyboard()
 
  print("GetInputKeyboard")

  -- Player moves right
  if (IsPressed(KEY_RIGHT))
  then
    moveDir = MOVE_RIGHT
  -- Player moves left
  elseif (IsPressed(KEY_LEFT))
  then
    moveDir = MOVE_LEFT
  -- Player does not move
  else
    moveDir = MOVE_IDLE
  end

  -- Player jumps
  if (IsPressed(KEY_JUMP))
  then
    jumpEnabled = true
  else
    jumpEnabled = false
  end

  -- Player stacks

end
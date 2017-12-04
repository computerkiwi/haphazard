--[[
FILE: GnomeMovement.lua
PRIMARY AUTHOR: Lya Vera

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

-- Connections
otherPlayer = nil

-- Variables
moveSpeed   = 2
jumpSpeed   = 4.5
fallSpeed   = 2

throwSpeed = 6

stackHeight = 0.5 -- Move to start function (set using gnome size)
stackTimer  = 0   -- Timer until player can stack again
stackParent = nil -- Player at the bottom of the stack for stacked update

lastPos     = nil -- Last position (for reviving); Vector type?
moveDir     = 0
lastDir		= 1

-- Bools
attackEnabled = false
jumpEnabled   = false
moveEnabled   = true
stackEnabled  = false
tossOther     = false
isTossed      = false
onGround      = true

-- Enums
PLAYER_LAYER  =  4
PLAYER_NUM    = -1 -- Set temporarily as invalid number
STACK_TIME    =  0.8

-- Move directions
MOVE_LEFT  = -1 -- Player moving left
MOVE_IDLE  =  0 -- Player idle
MOVE_RIGHT =  1 -- Player moving right

DEADZONE = 0.5 -- Joystick dead zone

-- Gamepad Buttons found under KeyMap.h
JUMP   = 0 -- A
ATTACK = 1 -- B
TOSS   = 3 -- Y

HORIZONTAL_AXIS = 0

function UpdateMovement(dt)
  HandleTimer(dt)

  -- Connections
  local playerBody = this:GetRigidBody()
  local playerTransform = this:GetTransform()

  local newVelocity  = playerBody.velocity
  local acceleration = playerBody.acceleration

  -- Calculate x velocity
  newVelocity.x = moveDir * moveSpeed

  -- Calculate y valocity
  if (jumpEnabled == true and onGround == true)
  then
    newVelocity.y = jumpSpeed
    jumpEnabled = false
    onGround = false
  end

  -- Update player velocity
  playerBody.velocity = newVelocity
end -- fn end

function HandleTimer(dt)
  -- Decrement timer
  if (stackTimer > 0)
  then
    stackTimer = stackTimer - dt
    -- Timer reaches 0, reset
    if (stackTimer <= 0)
    then
      stackTimer = 0
      -- Allow players to collide
      SetLayersColliding(PLAYER_LAYER, PLAYER_LAYER)
    end
  end
end -- fn end

-- More of Kieran's code
function StackedUpdate(dt)
  -- Update position based on parent
  local playerPos = this:GetTransform().position
  local otherPos = stackParent:GetTransform().position
  playerPos.y = otherPos.y + stackHeight
  playerPos.x = otherPos.x
  this:GetTransform().position = playerPos

  -- Update velocity based on parent
  local playerVelocity = this:GetRigidBody().velocity
  local otherVelocity = stackParent:GetRigidBody().velocity
  playerVelocity = otherVelocity

  -- Player jumps off
  if (jumpEnabled)
  then
    stackEnabled = false
    playerVelocity.y = jumpSpeed
    stackTimer = STACK_TIME -- Set timer
  end

  -- Update velocity
  this:GetRigidBody().velocity = playerVelocity
end -- fn end

-- Updates each frame
function Update(dt)
  -- Can we add playerID's to the player objects? :<

  -- TEMP move to start function
  -- Determine player once (which is why initial value is -1)
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
    GetInputKeyboard()
  end

  if (attackEnabled)
  then
    local script = this:GetScript("RedGnomeAttack.lua")
    local pos = this:GetTransform().position
    script.SetParentPos(pos)
  end

  -- Player is tossed
  --if (isTossed)
  --then
  --  TossUpdate(dt)
  -- Player is stacked
  if (stackEnabled)
  then
    StackedUpdate(dt)
  -- Update regular player movement
  else
    UpdateMovement(dt)
  end
end -- fn end

-- Other is a game object
function OnCollisionEnter(other)
  -- TODO: Change checks from name to tags

  -- Get name (for onGround)
  local otherName = other:GetName()

  -- Player collides with ground
  if (other:HasTag("Ground"))
  then
    onGround = true
  -- Player collides with other player
  elseif(onGround == false)
  then
    if (other:HasTag("Player") and stackEnabled == false)
    then
      StackPlayers(other)
    end
  end
end -- fn end

-- Kieran's stack code
function StackPlayers(other)
  -- Get transforms
  local playerTransform = this:GetTransform()
  local otherTransform = other:GetTransform()
    
  -- Get positions
  local playerPos = playerTransform.position
  local otherPos = otherTransform.position

  local snapDistance = 1 -- horizontal distance from other gnome
  local xDistance = playerPos.x -- x-axis distance between players

  -- How do I get the absolute value? Hmmmm
  if (playerPos.x > otherPos.x)
  then
    xDistance = playerPos.x - otherPos.x
  else
    xDistance = otherPos.x - playerPos.x
  end

  -- TODO: Change to use raycast downwards to check for gnome collision
  if (playerPos.y > otherPos.y)
  then
    -- Players are stacked
    stackEnabled = true

    -- Set bottom player as parent
    stackParent = other

    -- Don't detect collision between these layers
    SetLayersNotColliding(PLAYER_LAYER, PLAYER_LAYER)

    -- TEMP SOLUTION: make sure gnome is drawn in front
    --if (playerTransform.zLayer < otherTransform.zLayer)
    --then
    --  playerTransform.zLayer = otherTransform.zLayer - 1
    --end
  end
end -- fn end

-- TEMP
function SetKeyboardControls(name)
  if (name == "Player1")
  then
    PLAYER_NUM = 0
    otherPlayer = GameObject.FindByName("Player2")

    -- TEMPORARY
    KEY_JUMP   = 87 -- W
    KEY_DOWN   = 83 -- S
    KEY_LEFT   = 65 -- A
    KEY_RIGHT  = 68 -- D
    KEY_TOSS   = 84 -- T
    KEY_ATTACK = 89 -- Y

  else
    PLAYER_NUM = 1
    
    otherPlayer = GameObject.FindByName("Player1")

--    weapon = GameObject.FindByName("RedGnomeSword")

    -- TEMPORARY
		KEY_JUMP   = 265 -- Up
		KEY_DOWN   = 264 -- Down
		KEY_LEFT   = 263 -- Left
		KEY_RIGHT  = 262 -- Right
    KEY_TOSS   = 334 -- Numpad_Add
    KEY_ATTACK = 336 -- Numpad_Enter
  end
end -- fn end

-- Gamepad input
function GetInputGamepad()
  -- Player moves right
  if (GamepadGetAxis(PLAYER_NUM, HORIZONTAL_AXIS) > DEADZONE)
  then
    SetMoveDir(MOVE_RIGHT)
	lastDir = moveDir
  -- Player moves left
  elseif (GamepadGetAxis(PLAYER_NUM, HORIZONTAL_AXIS) < -DEADZONE)
  then
    SetMoveDir(MOVE_LEFT)
	lastDir = moveDir
  -- Player does not move
  else
    moveDir = MOVE_IDLE
  end


  -- Player jumps
  if (GamepadIsPressed(PLAYER_NUM, JUMP))
  then
    jumpEnabled = true
  else
    jumpEnabled = false
  end

  -- Player tosses
  if (GamepadIsPressed(PLAYER_NUM, TOSS))
  then
    tossEnabled = true
  else
    tossEnabled = false
  end
end -- fn end

-- Keyboard input
function GetInputKeyboard()
  -- Player moves right
  if (IsPressed(KEY_RIGHT))
  then
    SetMoveDir(MOVE_RIGHT)
	lastDir = moveDir
  -- Player moves left
  elseif (IsPressed(KEY_LEFT))
  then
    SetMoveDir(MOVE_LEFT)
	lastDir = moveDir
  -- Player does not move
  else
    SetMoveDir(MOVE_IDLE)
  end

  -- Player jumps
  if (IsPressed(KEY_JUMP))
  then
    jumpEnabled = true
  else
    jumpEnabled = false
  end

  -- Player attacks
  if (IsPressed(KEY_ATTACK))
  then
    attackEnabled = true
  else
    attackEnabled = false
  end

  -- TODO: MOVE TO OWN FUNCTION
  -- Player tosses (and is not stacked on other player)
  if (IsPressed(KEY_TOSS) and stackEnabled == false)
  then
    tossOther = true
    local script = otherPlayer:GetScript("GnomeMovement.lua")
    
    if (script.IsStacked())
    then    
      script.EnableToss()
      print("Enable Toss")
      script.SetMoveDir(moveDir)
      script.DisableStack()
      script.TossOnce()
      script.SetTimer()
      tossEnabled = false
    end
  else
    tossOther = false
--    local script = otherPlayer:GetScript("GnomeMovement.lua")
--    script.DisableToss()
  end
end -- fn end

function IsStacked()
  return stackEnabled
end -- fn end

function TossOnce()
  local playerBody = this:GetRigidBody()
  local newVelocity = playerBody.velocity
  newVelocity.y = throwSpeed

  newVelocity.x = moveDir * throwSpeed

  playerBody.velocity = newVelocity

  isTossed = false

end -- fn end

function TossUpdate(dt)
  -- Connections
  local playerBody = this:GetRigidBody()
  local playerTransform = this:GetTransform()

  local newVelocity = playerBody.velocity
  local acceleration = playerBody.acceleration

  -- Calculate x velocity
  newVelocity.x = moveDir * throwSpeed

  -- Calculate y valocity
  if (jumpEnabled == true)
  then
    newVelocity.y = jumpSpeed
    jumpEnabled = false
    isTossed = false
  end

  -- Update player velocity
  playerBody.velocity = newVelocity
end -- fn end

function EnableToss()
  isTossed = true
end -- fn end

function SetTimer()
  stackTimer = STACK_TIME
end -- fn end

function DisableToss()
  isTossed = false
end -- fn end

function DisableStack()
  stackEnabled = false
end -- fn end


function SetMoveDir(dir)
  moveDir = dir

  -- Flip sprite
  if(moveDir == -1)
  then
	  this:GetTransform().scale = vec3( -math.abs(this:GetTransform().scale.x), this:GetTransform().scale.y, 1 )
	--print(this:GetTransform().scale.x)
  elseif(moveDir == 1)
  then
	  this:GetTransform().scale = vec3( math.abs(this:GetTransform().scale.x), this:GetTransform().scale.y, 1 )
	 --print(this:GetTransform().scale.x)
  end

end -- fn end
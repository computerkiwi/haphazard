--[[
FILE: GnomeMovement.lua
PRIMARY AUTHOR: Lya Vera

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

-- Connections
otherPlayer = nil

-- Variables
moveSpeed	 = 2
jumpSpeed	 = 5.5
fallSpeed	 = 2

throwSpeed = 6

stackHeight = 0.5 -- Move to start function (set using gnome size)
stackTimer	= 0	 -- Timer until player can stack again
stackParent = nil -- Player at the bottom of the stack for stacked update

lastPos		 = nil -- Last position (for reviving); Vector type?
moveDir		 = 0
lastDir		= 1

AutoMove      = true
MaxActionTime = 0.45
MoveTimer     = 0.0

-- Bools
attackEnabled = false
jumpEnabled   = false
moveEnabled   = true
stackEnabled  = false
tossOther     = false
isTossed      = false
onGround      = true
unregulatedJumps = false

-- Enums
PLAYER_LAYER	=	4
PLAYER_NUM		= -1 -- Set temporarily as invalid number
STACK_TIME		=	0.8

-- Move directions
MOVE_LEFT	= -1 -- Player moving left
MOVE_IDLE	=	0 -- Player idle
MOVE_RIGHT =	1 -- Player moving right

DEADZONE = 0.5 -- Joystick dead zone

-- Gamepad Buttons found under KeyMap.h
JUMP	 = 0 -- A
ATTACK = 1 -- B
TOSS	 = 3 -- Y

HORIZONTAL_AXIS = 0

GROUND_CHECK_LENGTH = 0.05

WALK_FPS = 7

-- Returns (bool - found ground below), (number - y position of the top of the ground)
function CheckGround(count)
	local PLAYER_LAYER = 1 << 2
	local GROUND_LAYER = 1 << 3

	local pos	 = this:GetTransform().position
	local scale = this:GetCollider().dimensions
	
	local DOWN = vec2(0, -1)
	
	-- Make an array of raycast origins
	local origins = {}
	for i = 1, count do
		local fraction = (i - 1) / (count - 1)
	
		origins[i] = vec2(pos.x - scale.x / 2 + fraction * scale.x, pos.y + scale.y / 2)
	end
	
	-- Cast each raycast
	local casts = {}
	for i, origin in ipairs(origins) do
		casts[i] = Raycast.Cast(this:GetSpaceIndex(), origin, DOWN, scale.y + GROUND_CHECK_LENGTH, GROUND_LAYER )
	end
	
	-- Figure out wheter we hit and where the ground is.
	local hit = false
	local hitY = -999999999
	for i, cast in pairs(casts) do
		if (cast.gameObjectHit:IsValid())
		then
			hit = true
			hitY = math.max(hitY, cast.intersection.y)
		end
	end
	
	return hit, hitY
end

function UpdateMovement(dt)
	HandleTimer(dt)
	
	-- Connections
	local playerBody = this:GetRigidBody()
	local playerTransform = this:GetTransform()

	local newVelocity	= playerBody.velocity
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

	-- Toss the other gnome
	if (tossOther)
	then
		local otherScript = otherPlayer:GetScript("GnomeMovement.lua")
		
		-- Other gnome is stacked on this gnome
		if (otherScript.IsStacked())
		then
			otherScript.IsTossed()					 -- Set other gnome to isTossed
			print("Enable Toss")
			otherScript.SetMoveDir(moveDir)	-- Set other gnome's move direction
			otherScript.DisableStack()			 -- Set other gnome to "Not stacked"
			otherScript.SetTimer()					 -- Set other gnome's stack timer
			otherScript.TossOnce()					 -- Toss the other gnome
		end

		tossOther = false
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
	
	onGround = CheckGround(2) or unregulatedJumps
	
	-- Determine player once (which is why initial value is -1)
	if (PLAYER_NUM < 0)
	then
		local name = this:GetName()
		SetKeyboardControls(name)
	end

	--[[
	
	Disabled controllers because they always appear to be on on my computer. -Kieran

	-- Retrieve input
	if (GamepadsConnected() > 0)
	then
		print("Using gamepads")
		GetInputGamepad()
	else
	--]]
	GetInputKeyboard(dt)
	

--	if (attackEnabled)
--	then
--		local script = this:GetScript("RedGnomeAttack.lua")
--		script.SpawnWeapon()
--	end

	-- Player is tossed
	--if (isTossed)
	--then
	--	TossUpdate(dt)

	if (isTossed)
	then
		TossUpdate(dt)
	-- Player is stacked
	elseif (stackEnabled)
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

	-- Player collides with other player
	if (other:HasTag("Player") and stackEnabled == false)
	then
		StackPlayers(other)
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
		--	playerTransform.zLayer = otherTransform.zLayer - 1
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
		KEY_JUMP	 = 87 -- W
		KEY_DOWN	 = 83 -- S
		KEY_LEFT	 = 65 -- A
		KEY_RIGHT	= 68 -- D
		KEY_TOSS	 = 84 -- T
		KEY_ATTACK = 89 -- Y

	else
		PLAYER_NUM = 1
		
		otherPlayer = GameObject.FindByName("Player1")

--		weapon = GameObject.FindByName("RedGnomeSword")

		-- TEMPORARY
		KEY_JUMP	 = 265 -- Up
		KEY_DOWN	 = 264 -- Down
		KEY_LEFT	 = 263 -- Left
		KEY_RIGHT	= 262 -- Right
		KEY_TOSS	 = 334 -- Numpad_Add
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
		SetMoveDir(MOVE_IDLE)
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
function GetInputKeyboard(dt)

	if IsPressed(KEY.LeftControl) and IsTriggered(KEY.A)
	then
		AutoMove = not AutoMove;
	end

	if IsPressed(KEY.LeftControl) and IsTriggered(KEY.J)
	then
		unregulatedJumps = not unregulatedJumps;
	end

	if AutoMove == true
	then
		if MoveTimer <= 0
		then
			key_chart = 
			{
				-- Jump
				[1] = function() jumpEnabled = true end,
				[2] = function() jumpEnabled = true end,
				[3] = function() jumpEnabled = true end,
				[4] = function() jumpEnabled = true end,
				[5] = function() jumpEnabled = true end,

				-- D
				[5] = function() SetMoveDir(MOVE_RIGHT); lastDir = moveDir; end,
				[6] = function() SetMoveDir(MOVE_RIGHT); lastDir = moveDir; end,
				[7] = function() SetMoveDir(MOVE_RIGHT); lastDir = moveDir; end,
				[8] = function() SetMoveDir(MOVE_RIGHT); lastDir = moveDir; end,
				[9] = function() SetMoveDir(MOVE_RIGHT); lastDir = moveDir; end,

				-- A
				[10] = function() SetMoveDir(MOVE_LEFT); lastDir = moveDir; end,
				[11] = function() SetMoveDir(MOVE_LEFT); lastDir = moveDir; end,
				[12] = function() SetMoveDir(MOVE_LEFT); lastDir = moveDir; end,
				[13] = function() SetMoveDir(MOVE_LEFT); lastDir = moveDir; end,

				-- Attack
				[14] = function() end,
				[15] = function() end,
			}

			local key_to_simulate = math.random(1, 15);

			key_chart[key_to_simulate]();

			MoveTimer = MaxActionTime;
		else
			MoveTimer = MoveTimer - dt;
		end -- MoveTimer
	else
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
		--tossOther = true
		--local script = otherPlayer:GetScript("GnomeMovement.lua")
		
		--if (script.IsStacked())
		--then		
		--	script.IsTossed()
		--	print("Enable Toss")
		--	script.SetMoveDir(moveDir)
		--	script.DisableStack()
		--	script.TossOnce()
		--	script.SetTimer()
		--	tossEnabled = false

		tossOther = true
		else
		tossOther = false
	--		local script = otherPlayer:GetScript("GnomeMovement.lua")
	--		script.DisableToss()
		end -- Stack End
	end -- AutoMove
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
	
	isTossed = true
--	isTossed = false

end -- fn end

function TossUpdate(dt)
	HandleTimer(dt)

	-- Connections
	local playerBody = this:GetRigidBody()
	local playerTransform = this:GetTransform()

	local newVelocity = playerBody.velocity
	local acceleration = playerBody.acceleration

	-- Calculate x velocity
	newVelocity.x = moveDir * moveSpeed

	onGround = false

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

function IsTossed()
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
	local tex = this:GetSprite().textureHandler
	tex.fps = WALK_FPS;
	this:GetSprite().textureHandler = tex
	--print(this:GetTransform().scale.x)
	elseif(moveDir == 1)
	then
	this:GetTransform().scale = vec3( math.abs(this:GetTransform().scale.x), this:GetTransform().scale.y, 1 )
	local tex = this:GetSprite().textureHandler
	tex.fps = WALK_FPS;
	this:GetSprite().textureHandler = tex
	--print(this:GetTransform().scale.x)
	elseif(moveDir == 0)
	then
	local tex = this:GetSprite().textureHandler
	tex.fps = 0;
	tex.currentFrame = 0;
	this:GetSprite().textureHandler = tex
	end
end -- fn end

function GetMoveDir()
	if (moveDir == 0)
	then
		return lastDir
	else
		return moveDir
	end
end -- fn end
--[[
FILE: PlayerController.lua
PRIMARY AUTHOR: Kieran

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

STACK_HEIGHT = 0.5

jumpSpeed = 2

usingSprite1 = false
SPRITE_ID_1 = 80994
SPRITE_ID_2 = 154614

speed = 2

stacked = false
stackedObj = nil

PLAYER_LAYER = 4

UNSTACK_TIME = 0.1
unstackTimer = 0

grounded = false

function dumpTable(tab)
	for k,v in pairs(tab) do print(tostring(k) .. " - " .. type(v)) end
end

-- Sets variables based on which player we are.
function SpecifyPlayer()

	local name = this:GetName()

  GAMEPAD_JUMP   = 0  -- A
  GAMEPAD_ATTACK = 1  -- B
  GAMEPAD_LEFT   = 11 -- Left
  GAMEPAD_RIGHT  = 13 -- RIght

	if(name == "Player1")
	then
	
		otherPlayerName = "Player2"
		
		KEY_JUMP  = KEY.W
		KEY_DOWN  = KEY.S
		KEY_LEFT  = KEY.A
		KEY_RIGHT = KEY.D

    PLAYER_NUM     = 1  -- Player 2

		
	else
	
		otherPlayerName = "Player1"
		
		KEY_JUMP  = KEY.Up
		KEY_DOWN  = KEY.Down
		KEY_LEFT  = KEY.Left
		KEY_RIGHT = KEY.Right

    PLAYER_NUM     = 0  -- Player 1
	
	end

end

function StackedUpdate(dt)

	-- Set our position to be stacked.
	local thisPos  = this:GetTransform().position;
	local otherPos = stackedObj:GetTransform().position;
	thisPos.y = otherPos.y + STACK_HEIGHT
	thisPos.x = otherPos.x
	this:GetTransform().position = thisPos
	
	--[[
	NOTE: Doing this won't work:
	
	this:GetTransform().position.x = 5
	
	This is because we're getting the position but not setting it. 
	The position is copied, then we set x on the copy to 5.
	]]
	
	-- Keep us from randomly falling.
	local thisVel = this:GetRigidBody().velocity
	local otherVel = stackedObj:GetRigidBody().velocity
	thisVel.x = otherVel.x
	thisVel.y = otherVel.y
	
	--if (GamepadIsPressed(PLAYER_NUM, GAMEPAD_JUMP))
	if (IsPressed(KEY_JUMP))
  then
		stacked = false
		thisVel.y = jumpSpeed
		unstackTimer = UNSTACK_TIME
		
		PlaySound("stack_off.wav", 1, 1, false)
		
		PlaySound("jump.wav", 1, 1, false)
	end
	
	-- Set the velocity.
	this:GetRigidBody().velocity = thisVel
end

function UnstackedUpdate(dt)
	-- Deal with collision timer
	if (unstackTimer > 0)
	then
		unstackTimer = unstackTimer - dt
		
		-- If we tick to zero, set the collision and reset the timer.
		if (unstackTimer <= 0)
		then
			unstackTimer = 0
			SetLayersColliding(PLAYER_LAYER, PLAYER_LAYER)
		end
	end

	local rBody = this:GetRigidBody()
	local transform = this:GetTransform()
	tempVel = rBody.velocity
	--if (GamepadGetAxis(PLAYER_NUM, 0) > 0.05)
	if (IsPressed(KEY_RIGHT))
  then
		tempVel.x = speed
	--elseif(GamepadGetAxis(PLAYER_NUM, 0) < -0.05)
	elseif (IsPressed(KEY_LEFT))
  then
		tempVel.x = -speed
	else
		tempVel.x = 0
	end
	
	if (IsPressed(KEY_JUMP) and grounded)
	then
		tempVel.y = jumpSpeed
		grounded = false
		
		PlaySound("jump.wav", 1, 1, false)
	end
	
	rBody.velocity = tempVel
end

-- Called from the engine each frame.
function Update(dt)
	-- When the start function is implemented, this should be moved there.
	SpecifyPlayer()
	
	-- Call a different update based on our state.
	if (stacked)
	then
		StackedUpdate(dt)
	else
		UnstackedUpdate(dt)
	end
	
end

-- Called from the engine on collision.
function OnCollisionEnter(collidedObject)
	-- Get the transforms.
	local thisTransform = this:GetTransform()
	local otherTransform = collidedObject:GetTransform()

	-- Get the positions from the transforms.
	local thisPos  = thisTransform.position;
	local otherPos = otherTransform.position;

	local collName = collidedObject:GetName()
	
	if(collName == otherPlayerName and not stacked)
	then
		if (thisPos.y > otherPos.y)
		then
			stacked = true
			stackedObj = collidedObject
			SetLayersNotColliding(PLAYER_LAYER, PLAYER_LAYER)
			
			PlaySound("stack_on.wav", 1, 1, false)
			
			-- Make one gnome go in front of the other. (This doesn't work from engine-side as right now - 11/03/2017)
			if (thisTransform.zLayer > otherTransform.zLayer)
			then
				-- This is a bad way to do this. Layer should be more controlled than this.
				thisTransform.zLayer = otherTransform.zLayer - 1
			end
		end
	elseif (collName == "Ground")
	then
		grounded = true
	end
end
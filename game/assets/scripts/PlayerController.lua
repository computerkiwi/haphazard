
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
	if(name == "Player1")
	then
	
		otherPlayerName = "Player2"
		
		KEY_JUMP  = 87 -- W
		KEY_DOWN  = 83 -- S
		KEY_LEFT  = 65 -- A
		KEY_RIGHT = 68 -- D
		
	else
	
		otherPlayerName = "Player1"
		
		KEY_JUMP  = 265 -- Up
		KEY_DOWN  = 264 -- Down
		KEY_LEFT  = 263 -- Left
		KEY_RIGHT = 262 -- Right
	
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
	
	if (IsPressed(KEY_JUMP))
	then
		stacked = false
		thisVel.y = jumpSpeed
		unstackTimer = UNSTACK_TIME
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
	if (IsPressed(KEY_RIGHT))
	then
		tempVel.x = speed
	elseif(IsPressed(KEY_LEFT))
	then
		tempVel.x = -speed
	else
		tempVel.x = 0
	end
	
	if (IsPressed(KEY_JUMP)) --SPACE
	then
		tempVel.y = jumpSpeed
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

	if(collidedObject:GetName() == otherPlayerName and not stacked)
	then
		if thisPos.y > otherPos.y
		then
			stacked = true
			stackedObj = collidedObject
			SetLayersNotColliding(PLAYER_LAYER, PLAYER_LAYER)
			
			-- Make one gnome go in front of the other. (This doesn't work from engine-side as right now - 11/03/2017)
			if (thisTransform.zLayer > otherTransform.zLayer)
			then
				-- This is a bad way to do this. Layer should be more controlled than this.
				thisTransform.zLayer = otherTransform.zLayer - 1
			end
		end

	end
end

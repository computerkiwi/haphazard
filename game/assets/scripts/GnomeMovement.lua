--[[
FILE: GnomeMovement.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

--[[
What gnomes do:
Move				- Left/Right. No turn speeds. Ground materials are a thing. (Status effects). Stacked stuff: If on top, no movement. Only bottom gnome can move, set stacked flag.
Jump				- Heavier gravity on fall. Set velocity on jump. Dismounts from stacking. Moving is allowed in midair. Always same height. Hold down to continually jump
Stack				- On collision, try to stack. Threshold of vertical distance. 
Throw				- Just set velocity and dismount 
Die					- Hat floats around another player
Be a ghost			- Not platforming. Float around? Take away controls (other than ghosty stuff)
Respawn				- Stop being a ghost and become a real boy 
Pick things up		- Pick stuff up automatically and drop stuff on button press
Attack				- Handled per gnome, custom scripts

Auto Control 

Scripts on every gnome:
Input Handler
Gnome Movement (Movement and jumping)
Gnome Stacking (Stacking and throwing)
Gnome Health   (Gnome health and dead flag)
Gnome Ghost Movement
Gnome Status   (Where status is stored and how gnomes communicate their status)

]]

--[[ DEFINES ]]--

WALK_FPS = 7
DEADZONE = 0.5 -- Joystick dead zone
GROUND_CHECK_LENGTH = 0.05

-- Layers
PLAYER_LAYER = 1 << 2
GROUND_LAYER = 1 << 3

-- Move directions
MOVE_LEFT	= -1 -- Player moving left
MOVE_IDLE	=	0 -- Player idle
MOVE_RIGHT =	1 -- Player moving right



--[[ VARIABLES ]]--

-- Movement
moveSpeed	 = 2
moveDir		 = 0

-- Jumping
onGround      = false
jumpSpeed	 = 5.5
fallSpeed	 = 2

-- Flags
blockMovement = false
blockJump = false


-- Updates each frame
function Update(dt)		
	-- Update if they are on the ground
	onGround = CheckGround(2)

	-- Get Direction
	UpdateDir()

	-- Update Movement
	UpdateMovement(dt)

	-- Jump
	if (onGround == true and this:GetScript("InputHandler.lua").jumpPressed)
	then
		Jump()
	end

end -- fn end

function UpdateMovement(dt)
	-- Connections
	local playerBody = this:GetRigidBody()
	local playerTransform = this:GetTransform()

	local newVelocity	= playerBody.velocity

	newVelocity.x = moveDir * moveSpeed		-- Calculate x velocity
	playerBody.velocity = newVelocity		-- Update player velocity
end -- fn end

function Jump()
	PlaySound("jump.mp3", 0.1, 0.8, false)
	newVelocity.y = jumpSpeed
	jumpEnabled = false
	onGround = false
end

function UpdateDir()
	
	-- Call input to get horizontal axis
	moveDir = this:GetScript("InputHandler.lua").horizontalAxis

	if(moveDir == 0)
	then
		SetIdleAnimFPS()
		return
	end

	SetWalkAnimFPS()

	-- Flip sprite
	if(moveDir > 0)
	then
		this:GetTransform().scale = vec3( math.abs(this:GetTransform().scale.x), this:GetTransform().scale.y, 1 )
	else
		this:GetTransform().scale = vec3( -math.abs(this:GetTransform().scale.x), this:GetTransform().scale.y, 1 )
	end

end -- fn end

function SetWalkAnimFPS()
	local tex = this:GetSprite().textureHandler
	tex.fps = WALK_FPS
	this:GetSprite().textureHandler = tex
end

function SetIdleAnimFPS()
	local tex = this:GetSprite().textureHandler
	tex.fps = 0
	tex.currentFrame = 0
	this:GetSprite().textureHandler = tex
end


-- Returns (bool: found ground below), (number: y position of the top of the ground)
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
	
		origins[i] = vec2(pos.x - scale.x / 2 + fraction * scale.x, pos.y)
	end
	
	-- Cast each raycast
	local casts = {}
	for i, origin in ipairs(origins) do
		casts[i] = Raycast.Cast(this:GetSpaceIndex(), origin, DOWN, scale.y/2 + GROUND_CHECK_LENGTH, GROUND_LAYER )
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

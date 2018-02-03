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
!Input Handler
!Gnome Movement (Movement and jumping)
!Gnome Stacking (Stacking and throwing)
Gnome Health   (Gnome health and dead flag)
Gnome Ghost Movement
~Gnome Status   (Where status is stored and how gnomes communicate their status)

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

-- Size info
gnomeColliderYSize = 1

-- Updates each frame
function Update(dt)

	local status = this:GetScript("GnomeStatus.lua")

	-- Knockback is checked before ground update so it doesnt cancel on first update
	if(status.knockedBack == true)
	then
		if(onGround or status.stackedBelow ~= nil)
		then
			status.knockedBack = false
		end
	end

	-- Update if they are on the ground
	onGround = CheckGround(2)

	if(status.canMove == true and status.knockedBack == false)
	then
		-- Get Direction
		UpdateDir()

		-- Update Movement  (Not if stacked on top)
		if(status.stacked == false or status.stackedBelow == nil)
		then
			UpdateMovement(dt)
		end

		-- Jump
		if (onGround == true and this:GetScript("InputHandler.lua").jumpPressed)
		then
			Jump()
		end
	
		-- Is stacked (not bottom gnome) and jumps off (on jump press, not hold)
		if(status.stacked and this:GetScript("InputHandler.lua").onJumpPress and status.stackedBelow ~= nil)
		then
			this:GetScript("GnomeStack.lua"):Unstack()
			Jump()
		end

	end
	
	this:GetScript("GnomeStack.lua").UpdateParenting()

end -- fn end



function UpdateMovement(dt)
	-- Connections
	local playerBody = this:GetRigidBody()
	local playerTransform = this:GetTransform()

	local newVelocity	= playerBody.velocity

	local speed = moveSpeed
	if(this:GetScript("GnomeStatus.lua").specialMove)
	then 
		speed = speed * this:GetScript("GnomeStatus.lua").specialMoveScale
	end

	newVelocity.x = moveDir * speed		-- Calculate x velocity
	playerBody.velocity = newVelocity	-- Update player velocity
end -- fn end

function Jump()
	PlaySound("jump.mp3", 0.1, 0.8, false)

	local speed = jumpSpeed
	if(this:GetScript("GnomeStatus.lua").specialJump)
	then
		speed = speed * this:GetScript("GnomeStatus.lua").specialJumpScale
	end

	local newVelocity = this:GetRigidBody().velocity
	newVelocity.y = speed
	this:GetRigidBody().velocity = newVelocity

	onGround = false

	this:GetScript("GnomeAbilities.lua").Jump()
end

function Knockback(dir, force)

	this:GetScript("GnomeStatus.lua").knockedBack = true
	this:GetRigidBody().velocity = vec3(dir.x * force, dir.y * force, 0)
	onGround = false

end

function UpdateDir()
	
	local input = this:GetScript("InputHandler.lua")

	-- Call input to get horizontal axis
	moveDir = input.horizontalAxis

	if(moveDir == 0)
	then
		SetIdleAnimFPS()
		return
	end

	local dir = vec2(1,0)
	if(moveDir == -1)
	then
		dir = vec2(-1,0)
	end

	this:GetScript("ProjectileSpawner.lua").SetDir(dir)

	if(this:GetScript("GnomeStatus.lua").stacked)
	then
		this:GetScript("ProjectileSpawner.lua").SetAim(vec2(input.horizontalAxis, input.verticalAxis))
	else
		this:GetScript("ProjectileSpawner.lua").SetAim(dir)
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
	scale.y = gnomeColliderYSize

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

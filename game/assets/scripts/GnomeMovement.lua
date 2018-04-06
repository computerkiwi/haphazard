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
DUST_PARTICLE_OFFSET = -0.5
STATUE_MOVE_COOLDOWN = 1

-- Layers
PLAYER_LAYER = 1 << 2
GROUND_LAYER = 1 << 3

-- Move directions
MOVE_LEFT	= -1 -- Player moving left
MOVE_IDLE	=	0 -- Player idle
MOVE_RIGHT =	1 -- Player moving right

--[[ VARIABLES ]]--

-- Movement
moveSpeed	 = 2.5
moveDir		 = 0
facing       = 1
statueXSpeed = 0.4
statueYVelocity = 2
statueMoveTimer = 0

-- Jumping
onGround      = false
jumpSpeed	 = 5.5
fallSpeed	 = 2
ledgeForgivenessTimer = 0
LEDGE_FORGIVENESS_TIME = 0.15

-- Flags
blockMovement = false
blockJump = false

-- Particle Objects
dustParticleObject = nil
dustParticlesEnabled = nil -- Initialized in start.

-- Prefab files
WALK_PREFAB_NAME = "assets/prefabs/DustParticles.json"
JUMP_PREFAB_NAME = "assets/prefabs/DustJump.json"

local dustParticleCount = 0
function SetDustEnabled(shouldBeEnabled)
	-- Don't bother changing if our status is already in the state we want it in.
	if (dustParticlesEnabled == shouldBeEnabled)
	then
		return
	end
	
	-- Update the status.
	dustParticlesEnabled = shouldBeEnabled
	
	local pSystem = dustParticleObject:GetParticleSystem()
	local settings = pSystem.settings
	settings.ParticlesPerEmission = shouldBeEnabled and dustParticleCount or 0
	pSystem.settings = settings
	
end

function InitDustParticles()
	local particleObjectName = "__" .. this:GetName() .. "_GNOME_DUST_PARTICLES"
	
	-- Delete the particle object if it already exists.
	local existingParticleObject = GameObject.FindByName(particleObjectName)
	if (existingParticleObject:IsValid())
	then
		existingParticleObject:Delete()
	end
	

	-- Create the dust particle object.
	dustParticleObject = GameObject.LoadPrefab(WALK_PREFAB_NAME)
	dustParticleObject:SetName(particleObjectName)
	local transform = dustParticleObject:GetTransform()
	
	-- Attach it to the gnome.
	transform.parent = this
	transform.localPosition = vec2(0, DUST_PARTICLE_OFFSET)
	
	-- Initially disable the dust.
	dustParticlesEnabled = false
	local pSystem = dustParticleObject:GetParticleSystem()
	local settings = pSystem.settings
	-- Save the standard amount of particles per emission and stop emitting particles.
	dustParticleCount = settings.ParticlesPerEmission
	settings.ParticlesPerEmission = 0
	pSystem.settings = settings
end

function Start()
	InitDustParticles()
end

-- Updates each frame
function Update(dt)

	local status = this:GetScript("GnomeStatus.lua")

	local transform = this:GetTransform()

	-- Knockback is checked before ground update so it doesnt cancel on first update
	if(status.knockedBack == true or status.tossed == true)
	then
		if(status.stackedBelow ~= nil or this:GetDynamicCollider().colliderData:IsCollidingWithLayer(GROUND_LAYER) )
		then
			status.knockedBack = false
			status.tossed = false
		end
	end

	-- Update if they are on the ground
	ledgeForgivenessTimer = ledgeForgivenessTimer - dt
	if (CheckGround(2))
	then
		onGround = true
	else
		if (onGround == true)
		then
			ledgeForgivenessTimer = LEDGE_FORGIVENESS_TIME 
		end		
		onGround = false                  
	end

	if(status.canMove == true and status.knockedBack == false and status.isStatue == false and status.killedByChaseBox == false)
	then
		-- Get Direction
		UpdateDir()

		-- Update Movement  (Not if stacked on top)
		if(status.stacked == false or status.stackedBelow == nil)
		then
			UpdateMovement(dt)
		end

		-- Jump
		if ((onGround == true or ledgeForgivenessTimer > 0 or _G.GOD_MODE) and this:GetScript("InputHandler.lua").jumpPressed)
		then
			Jump()
		end
	
		CheckToss()

		-- Is stacked (not bottom gnome) and jumps off (on jump press, not hold)
		if(status.stacked and this:GetScript("InputHandler.lua").onJumpPress and status.stackedBelow ~= nil)
		then
			this:GetScript("GnomeStack.lua"):Unstack()
			Jump()
		end

	elseif(status.isStatue == true and status.killedByChaseBox == false)
	then
		StatueUpdate(dt)
	end
	
	if (status.stacked)
	then
		SetDustEnabled(false)
	end
	
	-- Make sure we don't have any lingering effects from toss rotation.
	if(status.tossed == false)
	then
		transform.rotation = 0
	end

	this:GetScript("GnomeStack.lua").UpdateParenting()

end -- fn end



function UpdateMovement(dt)
	-- Connections
	local playerBody = this:GetRigidBody()

	local newVelocity	= playerBody.velocity
	local transform = this:GetTransform()

	local speed = moveSpeed
	if(this:GetScript("GnomeStatus.lua").specialMove)
	then 
		speed = speed * this:GetScript("GnomeStatus.lua").specialMoveScale
	end

	if(this:GetScript("GnomeStatus.lua").tossed)
	then
		local rotateMult = 1000
		if (newVelocity.x >= 0)
		then
			rotateMult = -rotateMult
		end
		
		transform.rotation = transform.rotation + dt * rotateMult
		-- Tossed movement
		newVelocity.x = newVelocity.x + moveDir * speed -- Calculate x velocity ()

		if(newVelocity.x > speed) then newVelocity.x = speed end
		if(newVelocity.x < -speed) then newVelocity.x = -speed end

		playerBody.velocity = newVelocity	-- Update player velocity
	else
		newVelocity.x = moveDir * speed		-- Calculate x velocity
		playerBody.velocity = newVelocity	-- Update player velocity
	end
	
	-- Update dust particles
	SetDustEnabled(moveDir ~= 0 and onGround)
	
end -- fn end

function Jump()
	-- Get rid of ledge forgiveness
	ledgeForgivenessTimer = 0
	
	local speed = jumpSpeed
	if(this:GetScript("GnomeStatus.lua").specialJump)
	then
		speed = speed * this:GetScript("GnomeStatus.lua").specialJumpScale
	end

	local newVelocity = this:GetRigidBody().velocity
	newVelocity.y = speed
	this:GetRigidBody().velocity = newVelocity

	onGround = false

	local jumpParticle = GameObject.LoadPrefab(JUMP_PREFAB_NAME)
	local pos = this:GetTransform().position
	jumpParticle:GetTransform().position = vec2(pos.x, pos.y + DUST_PARTICLE_OFFSET)
	
	this:GetScript("GnomeAbilities.lua").Jump()
end

function CheckToss()
	local input = this:GetScript("InputHandler.lua")
	local status = this:GetScript("GnomeStatus.lua")

	if(input.tossPressed and status.stackedAbove ~= nil)
	then
		-- Toss
		PlaySound("Grunt4.wav", 1, 1, false)
		local above = status.stackedAbove
		above:GetScript("GnomeStack.lua").Unstack()
		above:GetScript("GnomeStatus.lua").tossed = true
		above:GetScript("GnomeMovement.lua").Jump()

		local newVelocity = above:GetRigidBody().velocity
		
		dir = moveDir
		if(dir == 0) 
		then 
			dir = this:GetTransform().scale.x
			if(math.abs(dir) ~= 1)
			then
				dir = dir / math.abs(dir)
			end
		end

		newVelocity.x = moveSpeed * dir
		above:GetRigidBody().velocity = newVelocity
	end
end

function Knockback(dir, force)

	this:GetScript("GnomeStatus.lua").knockedBack = true
	this:GetRigidBody().velocity = vec3(dir.x * force, dir.y * force, 0)
	this:GetTransform().position = vec2(this:GetTransform().position.x, this:GetTransform().position.y + 0.1)
	onGround = false

end

function UpdateDir()
	
	local input = this:GetScript("InputHandler.lua")

	-- Call input to get horizontal axis
	moveDir = input.horizontalAxis
	if (moveDir > 0)
	then
		moveDir = MOVE_RIGHT
		facing = moveDir
	elseif(moveDir < 0)
	then
		moveDir = MOVE_LEFT
		facing = moveDir
	end


	if(moveDir == 0)
	then
		if (facing == MOVE_RIGHT)
		then
			this:GetScript("ProjectileSpawner.lua").SetAim({x = 1, y = 0})
		else
			this:GetScript("ProjectileSpawner.lua").SetAim({x = -1, y = 0})
		end

		SetIdleAnimFPS()
		return
	end

	local dir = vec2(1,0)
	if(moveDir < 0)
	then
		dir = vec2(-1,0)
	end

	this:GetScript("ProjectileSpawner.lua").SetDir(dir)

	-- If we're stacked and aiming set the aim.
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
	if(this:GetScript("GnomeStatus.lua").isStatue) then return end

	local tex = this:GetSprite().textureHandler
	tex.fps = WALK_FPS
	this:GetSprite().textureHandler = tex
end

function SetIdleAnimFPS()
	if(this:GetScript("GnomeStatus.lua").isStatue) then return end

	local tex = this:GetSprite().textureHandler
	tex.fps = 0
	tex.currentFrame = 0
	this:GetSprite().textureHandler = tex
end


-- Returns (bool: found ground below), (number: y position of the top of the ground)
function CheckGround(count)
	local pos	 = this:GetTransform().position
	local scale = this:GetCollider().dimensions

	local WIDTH_FRACTION = 0.9
	local DOWN = vec2(0, -1)
	
	-- Make an array of raycast origins
	local origins = {}
	for i = 1, count do
		local fraction = (i - 1) / (count - 1)
	
		origins[i] = vec2(pos.x + WIDTH_FRACTION *(fraction * scale.x - scale.x / 2), pos.y)
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
			this:GetTransform().parent = cast.gameObjectHit
			hit = true
			hitY = math.max(hitY, cast.intersection.y)
		end
	end
	
	if (hit == false)
	then
		this:GetTransform().parent = GameObject(0)
	end
	
	
	return hit, hitY
end

function OnCollisionEnter(other)
	if(this:GetDynamicCollider().colliderData:IsCollidingWithLayer(GROUND_LAYER))
	then
		this:GetScript("GnomeStatus.lua").knockedBack = false
		this:GetScript("GnomeStatus.lua").tossed = false
	end
end


function StatueUpdate(dt)
	local playerBody = this:GetRigidBody()
	statueMoveTimer = statueMoveTimer - dt

	UpdateDir()

	if(moveDir ~= 0 and onGround)
	then
		if(statueMoveTimer <= 0)
		then
			this:GetScript("GnomeHealth.lua").ChipStatue()
			statueMoveTimer = STATUE_MOVE_COOLDOWN
		end

		local speed = statueXSpeed
		local jump = statueYVelocity

		local newVelocity = vec3(moveDir * speed, jump, 0)

		playerBody.velocity = newVelocity	-- Update player velocity
		
		-- Update dust particles
		SetDustEnabled(moveDir ~= 0 and onGround)
	else
		SetDustEnabled(false)
		
		if(onGround)
		then
			playerBody.velocity = vec3(0, playerBody.velocity.y, 0)
		end
	end
end
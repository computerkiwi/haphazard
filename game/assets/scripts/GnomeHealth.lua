--[[
FILE: GnomeHealth.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

STATUE_HIT_POINTS = 7
DEAD_GNOME_LAYER = 1 << 11 --2048
ALLY_PROJECTILE_LAYER = 1 << 5 --32

ENEMY_LAYER = 16;

health = 6
statueHitPoints = 7

healthBar = nil
invulTime = 0

INVULN_BLINK_RATE = 30
DAMAGE_FLASH_TIME = 0.1

KNOCKBACK_ANGLE = 40
KNOCKBACK_FORCE = 5

INVULNERABLE_TIME = 2

LEVEL = "Level1.json"

local sqrt2 = math.sqrt(2)

Sprite_Red_Statue = "Death_Red.json"
Sprite_Green_Statue = "Death_Green.json"
Sprite_Blue_Statue = "Death_Blue.json"
Sprite_Yellow_Statue = "Death_Yellow.json"


UI_SHAKE_AMOUNT = 1

local DAMAGE_SHAKE = 0.3
local DEATH_SHAKE = 0.6

local DEATH_NOTICE = nil

DEATH_NOTICE_OFFSET_Y = 1

DEATH_PARTICLES_OBJECT = "assets/prefabs/DeathEffect.json"
CHIP_PARTICLES_OBJECT = "assets/prefabs/StatueChipEffect.json"
RESPAWN_PARTICLES_OBJECT = "assets/prefabs/RespawnEffect.json"

function SpawnAndAttachObject(prefabName, parentObj)
	local obj = GameObject.LoadPrefab(prefabName)
	if (obj:IsValid())
	then
		obj:GetTransform().parent = parentObj
		
		obj:GetTransform().localPosition = vec2(0,0)

		return obj
	else
		return nil
	end
end

function NewPlaysoundFunction(...)
  local sounds = {...}
  return function()
    local soundName = sounds[math.random(1, #sounds)]
	
    PlaySound(soundName, 1, 1, false)
  end
end

PlayCrackSound = NewPlaysoundFunction(
  "statue_crack_01.wav",
  "statue_crack_02.wav")

function GetHealthBar()
  healthBar = GameObject.FindByName(this:GetName().."Healthbar") 
  if (healthBar:IsValid()) 
  then 
		return true
  else
		healthBar = nil
		return false
	end
end

function Start()
	GetHealthBar()
end

function Update(dt)
  
	if (healthBar == nil)
	then
		GetHealthBar()
	end
	
  if(invulTime > 0)
  then
    invulTime = invulTime - dt
		
		-- Handle blinking
		local blinkOn = (math.sin(invulTime * INVULN_BLINK_RATE) > 0)
		local sprite = this:GetSprite()
		local tempColor = sprite.color
		-- Set alpha based on blink.
		if (blinkOn)
		then
			tempColor.w = 1
		else
			tempColor.w = 0.6
		end
		
		-- Set red based on flash.
		if (INVULNERABLE_TIME - invulTime < DAMAGE_FLASH_TIME)
		then
			tempColor.x = 0.94
			tempColor.y = 0.26
			tempColor.z = 0.22
		else
			tempColor.x = 1
			tempColor.y = 1
			tempColor.z = 1
		end
		sprite.color = tempColor
		
  elseif(invulTime <= 0)
	then
	
		-- Reset the alpha.
		local sprite = this:GetSprite()
		local tempColor = sprite.color
		tempColor.w = 1
		sprite.color = tempColor
		
		-- Set invulTime to 0
		invulTime = 0
	end
  
end

function TypeName(type)
	if(type == 1)
	then
		return "Red"
	elseif(type == 2)
	then
		return "Green"
	elseif(type == 3)
	then
		return "Blue"
	elseif(type == 4)
	then
		return "Yellow"
	end
end

function IsDead()

	return health <= 0

end

function Damage(damageAmount, damageSourceLocation)
	
	if(_G.GOD_MODE)
	then

		return

	end
		
	-- Actually deal the damage
    health = health - damageAmount
	if(health < 0)
	then 
		health = 0
	end

	if(this:GetScript("GnomeStatus.lua").killedByChaseBox)
	then
		return
	end
	
	-- Apply player knockback.
	if(damageSourceLocation ~= nil)
	then
		local movementScript = this:GetScript("GnomeMovement.lua")
		local pos = this:GetTransform().position
		local knockbackDir = {x = math.cos(math.rad(KNOCKBACK_ANGLE)), y = math.sin(math.rad(KNOCKBACK_ANGLE))}
		if (pos.x < damageSourceLocation.x)
		then
			knockbackDir.x = -knockbackDir.x
		end
		movementScript.Knockback(knockbackDir, KNOCKBACK_FORCE)
	end
	
	-- Screenshake
	_G.Screenshake(DAMAGE_SHAKE)
	PlaySound("gnome_hurt_01.wav", 1, 1, false)
	
	-- Shake the UI
	if (healthBar ~= nil)
	then
		local healthBarScript = healthBar:GetScript("Level1GUI.lua")
		healthBarScript.Shake(UI_SHAKE_AMOUNT)
	end
	
	-- Set the invulnerability timer
	invulTime = INVULNERABLE_TIME

	-- Check if the player died.
	if(IsDead())
	then
		-- Is dead
		this:GetScript("GnomeStatus.lua").isStatue = true
		statueHitPoints = STATUE_HIT_POINTS
		this:GetCollider().collisionLayer = CollisionLayer(DEAD_GNOME_LAYER)

		DEATH_NOTICE = SpawnAndAttachObject("assets/prefabs/Death_Wiggler.json", this)
		DEATH_NOTICE:GetTransform().localPosition = vec2(0, DEATH_NOTICE_OFFSET_Y)

		SpawnAndAttachObject(DEATH_PARTICLES_OBJECT, this)

    _G.Screenshake(DEATH_SHAKE)
    PlaySound("gnome_death_01.wav", 1, 1, false)
    
		SetStatueSprite()
	end
end

function OnCollisionEnter(other)

  if(invulTime <= 0 and this:GetDynamicCollider().colliderData:IsCollidingWithLayer(ENEMY_LAYER))
  then
	if(this:GetScript("GnomeStatus.lua").isStatue == false)
	then
		Damage(1, other:GetTransform().position)
	end
  end

  if(IsDead() and other:GetCollider().collisionLayer.layer == ALLY_PROJECTILE_LAYER)
  then

	other:GetScript("ProjectileInfo.lua").DidHit()

	ChipStatue()

	local direction = vec2(0, sqrt2)
	DeadHitKnockbackForce = 1

	if(other:GetTransform().position.x >= this:GetTransform().position.x)
	then

		direction.x = -sqrt2

	else

		direction.x = sqrt2

	end

	this:GetScript("GnomeMovement.lua").Knockback(direction, DeadHitKnockbackForce)

  end

end


function SetStatueSprite()
	local type = this:GetScript("GnomeStatus.lua").GnomeType

	print("IM DEAD" .. type)	

	if(this:GetScript("GnomeStatus.lua").isStatue)
	then		
		print("IM A STATUE OF TYPE " .. type .." To sprite " ..Sprite_Red_Statue)
		-- Set sprite
		if(type == 1)	-- Red
		then
			this:GetSprite().id = Resource.FilenameToID(Sprite_Red_Statue)
		elseif(type == 2)	-- Green
		then
			this:GetSprite().id = Resource.FilenameToID(Sprite_Green_Statue)
		elseif(type == 3)	-- Blue
		then
			this:GetSprite().id = Resource.FilenameToID(Sprite_Blue_Statue)
		elseif(type == 4)	-- Yellow
		then
			this:GetSprite().id = Resource.FilenameToID(Sprite_Yellow_Statue)
		end

		local tex = this:GetSprite().textureHandler
		tex.currentFrame = 0
		this:GetSprite().textureHandler = tex
	else
		this:GetScript("GnomeAbilities.lua").SetType(type) -- Reset sprite
	end

end

function ChipStatue()
	statueHitPoints = statueHitPoints - 1
	
	local tex = this:GetSprite().textureHandler
  local lastFrame = tex.currentFrame
	if(statueHitPoints < 12)
	then
		tex.currentFrame = 4 - math.ceil(statueHitPoints / 3)
		this:GetSprite().textureHandler = tex
	else
		tex.currentFrame = 0
		this:GetSprite().textureHandler = tex
	end
  
  -- Chipping effects on visual chip
  if (lastFrame ~= tex.currentFrame)
  then
    PlayCrackSound()
    local chip = SpawnAndAttachObject(CHIP_PARTICLES_OBJECT, this)
  end

	if(statueHitPoints <= 0)
	then
		this:GetScript("GnomeStatus.lua").isStatue = false
		SetStatueSprite()

    PlaySound("statue_shatter_01.wav", 1, 1, false)
    PlaySound("gnome_respawn_01.wav", 1, 1, false)
    
		health = 6

		this:GetScript("GnomeMovement.lua").Jump()

		DEATH_NOTICE:Destroy()
		DEATH_NOTICE = nil

		SpawnAndAttachObject(RESPAWN_PARTICLES_OBJECT, this)

	end
end

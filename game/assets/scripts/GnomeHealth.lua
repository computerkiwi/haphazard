--[[
FILE: GnomeHealth.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

STATUE_HIT_POINTS = 14

ENEMY_LAYER = 16;

health = 6
statueHitPoints = 14

healthBar = nil
invulTime = 0

INVULN_BLINK_RATE = 30
DAMAGE_FLASH_TIME = 0.1

KNOCKBACK_ANGLE = 40
KNOCKBACK_FORCE = 5

INVULNERABLE_TIME = 2

LEVEL = "Level1.json"

Sprite_Red_Statue = "Death_Red.json"
Sprite_Green_Statue = "Death_Green.json"
Sprite_Blue_Statue = "Death_Blue.json"
Sprite_Yellow_Statue = "Death_Yellow.json"


UI_SHAKE_AMOUNT = 1

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
		
  elseif(invulTime < 0)
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

function Damage(damageAmount, damageSourceLocation)
	-- Actually deal the damage
  health = health - damageAmount
	PlaySound("Grunt7.wav", 1, 1, false)
	local type = this:GetScript("GnomeStatus.lua").GnomeType
	local particle = GameObject.LoadPrefab("assets/prefabs/Particles_" .. TypeName(type) .. "_Hit.json")
	particle:GetTransform().position = this:GetTransform().position
	
	-- Apply player knockback.
	local movementScript = this:GetScript("GnomeMovement.lua")
	local pos = this:GetTransform().position
	local knockbackDir = {x = math.cos(math.rad(KNOCKBACK_ANGLE)), y = math.sin(math.rad(KNOCKBACK_ANGLE))}
	if (pos.x < damageSourceLocation.x)
	then
		knockbackDir.x = -knockbackDir.x
	end
	movementScript.Knockback(knockbackDir, KNOCKBACK_FORCE)
	
	-- Shake the UI
	if (healthBar ~= nil)
	then
		local healthBarScript = healthBar:GetScript("Level1GUI.lua")
		healthBarScript.Shake(UI_SHAKE_AMOUNT)
	end
	
	-- Set the invulnerability timer
	invulTime = INVULNERABLE_TIME

	-- Check if the player died.
	if(health <= 0)
	then
		-- Is dead
		this:GetScript("GnomeStatus.lua").isStatue = true
		statueHitPoints = STATUE_HIT_POINTS

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
	
	if(statueHitPoints < 12)
	then
		local tex = this:GetSprite().textureHandler
		tex.currentFrame = 4 - math.ceil(statueHitPoints / 3)
		this:GetSprite().textureHandler = tex
	else
		tex.currentFrame = 0
		this:GetSprite().textureHandler = tex
	end

	if(statueHitPoints <= 0)
	then
		this:GetScript("GnomeStatus.lua").isStatue = false
		SetStatueSprite()

		health = 6

		this:GetScript("GnomeMovement.lua").Jump()
	end
end

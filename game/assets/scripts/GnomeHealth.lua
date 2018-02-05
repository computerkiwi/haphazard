--[[
FILE: GnomeHealth.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]


ENEMY_LAYER = 16;

health = 6
healthBar = nil
invulTime = 0

INVULN_BLINK_RATE = 30
DAMAGE_FLASH_TIME = 0.1

KNOCKBACK_ANGLE = 40
KNOCKBACK_FORCE = 5

INVULNERABLE_TIME = 2

LEVEL = "Level1.json"

function Start()
	healthBar = GameObject.FindByName(this:GetName().."Healthbar")
	if (healthBar:IsValid())
	then
		local th = healthBar:GetSprite().textureHandler
		th.currentFrame = health
		healthBar:GetSprite().textureHandler = th
	end
end

function Update(dt)
  
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
function Damage(damageAmount, damageSourceLocation)
	-- Actually deal the damage
  health = health - damageAmount
	PlaySound("gnome_injure.mp3", 1.5, 1, false)

	-- Update the healthbar
	if (healthBar:IsValid())
	then
		local th = healthBar:GetSprite().textureHandler
		th.currentFrame = health
		healthBar:GetSprite().textureHandler = th
	end
	
	-- Apply player knockback.
	local movementScript = this:GetScript("GnomeMovement.lua")
	local pos = this:GetTransform().position
	local knockbackDir = {x = math.cos(math.rad(KNOCKBACK_ANGLE)), y = math.sin(math.rad(KNOCKBACK_ANGLE))}
	if (pos.x < damageSourceLocation.x)
	then
		knockbackDir.x = -knockbackDir.x
	end
	movementScript.Knockback(knockbackDir, KNOCKBACK_FORCE)
	
	-- Set the invulnerability timer
	invulTime = INVULNERABLE_TIME

	-- Check if the player died.
	if(health <= 0)
	then
		if(this:GetName() == "Player1" and not GameObject.FindByName("Player2"):IsActive())
		then
			Engine.LoadLevel(LEVEL)
		elseif(this:GetName() == "Player2" and not GameObject.FindByName("Player1"):IsActive())
		then
			Engine.LoadLevel(LEVEL)
		end

		this:Deactivate();
	end
end

function OnCollisionEnter(other)

  if(invulTime <= 0 and this:GetDynamicCollider().colliderData:IsCollidingWithLayer(ENEMY_LAYER))
  then
		Damage(1, other:GetTransform().position)
	end

end

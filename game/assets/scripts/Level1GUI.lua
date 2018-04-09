--[[
FILE: Level1GUI.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]



PlayerName = "Player1"
Player = nil

cam = nil

offset = vec2(0,0)
local startScaleX = 1.8
local startScaleY = (256 / 512) * startScaleX

local SPACING = 0

local LEFT_X = -5.8 + startScaleX / 2
local Y_OFFSET = 2.8 - startScaleY / 2
local X_OFFSET = LEFT_X

local hearts = nil

--local XOffset
--local YOffset

shakeAmount = 0
SHAKE_SCALE = 0.2
SHAKE_FALLOFF = 0.3

function SpawnAndAttachObject(prefabName, resetPosition)
	resetPosition = resetPosition or false

	local obj = GameObject.LoadPrefab(prefabName)
	if (obj:IsValid())
	then
		obj:GetTransform().parent = this
		if (resetPosition)
		then
			obj:GetTransform().localPosition = vec2(0,0)
		end
	
		return obj
	else
		return nil
	end
end


function ColorIndex()
  return tonumber(string.match(PlayerName, "Player(.)")) - 1
end

-- Returns the frame index of the correctly colored healthbar.
function ColorFrame(isDead)
  isDead = isDead or false
  
  local playerNum = ColorIndex() * 2
  
  if(not isDead)
  then
    playerNum = playerNum + 1
  end
  
  return playerNum
end

function Shake(amount)
	shakeAmount = shakeAmount + amount
end

function Start()
	cam = GameObject.FindByName("Main Camera")
  --[[ Changed to specific scale to avoid UI getting larger whenever
       I zoom out, edit a level, save, and load. -- Lya ]]
  if(this:GetName() == "GemCounter")
  then
    this:Destroy()
  end

  X_OFFSET = LEFT_X + (startScaleX + SPACING) * ColorIndex() + startScaleX * 0.5
  
  -- Change to the new sprite.
  local spr = this:GetSprite()
  spr.id = Resource.FilenameToID("HUD_Healthbars.json")
  local th = spr.textureHandler
  th.currentFrame = ColorFrame()
  spr.textureHandler = th
  
  hearts = SpawnAndAttachObject("assets/prefabs/HealthbarHearts.json", true)
  
	--offset = this:GetTransform().position
end

-- Range [0, 1]
function SetBarAmount(amount)
  local Y_SCALE_FACTOR = 0.4
  local X_SCALE_FACTOR = 0.8

  local thisScale = this:GetTransform().scale
  
  -- What the scale is at full health.
  local fullScale = {x = X_SCALE_FACTOR * thisScale.x, y = Y_SCALE_FACTOR * thisScale.y}
  
  bar:GetTransform().scale = vec3(fullScale.x * amount, fullScale.y, 1)
  bar:GetTransform().localPosition = vec2(((fullScale.x - fullScale.x * amount) / 2) * -1, 999999)
  
end

function UpdateHeartsPos()
  local HEIGHT_PERCENT = (64 / 256)
  
  local THIS_SCALE_X = this:GetTransform().scale.x
  local THIS_SCALE_Y = this:GetTransform().scale.y
  
  local SCALE_Y = THIS_SCALE_Y * HEIGHT_PERCENT
  local SCALE_X = SCALE_Y * (224 / 64)

  local OFFSET_X = ((335 / 512) - 0.5) * THIS_SCALE_X
  local OFFSET_Y = (( 97 / 256) - 0.5) * THIS_SCALE_Y
  
  hearts:GetTransform().scale = vec3(SCALE_X, SCALE_Y, 0)
  hearts:GetTransform().localPosition = vec2(OFFSET_X, OFFSET_Y)
end

function RotatedVec(x, y, rotation)
	local cosTheta = math.cos(math.rad(rotation))
	local sinTheta = math.sin(math.rad(rotation))

	return vec2(cosTheta * x - sinTheta * y, sinTheta * x + cosTheta * y)
end

function LateUpdate(dt)

	Player = GameObject.FindByName(PlayerName)

	local camRot = cam:GetTransform().rotation
	
	-- Change shake amount
	shakeAmount = math.max(math.lerp(shakeAmount, 0 , SHAKE_FALLOFF), 0)
	
	-- Calculate shake
	local shakeAngle = math.random() * math.rad(360)
	local shakeX = math.cos(shakeAngle) * shakeAmount * SHAKE_SCALE
	local shakeY = math.sin(shakeAngle) * shakeAmount * SHAKE_SCALE
	
	local zoomScale = cam:GetCamera().zoom / 5
	local thisTransform = this:GetTransform()
	local pos = RotatedVec((X_OFFSET + shakeX) * zoomScale, (Y_OFFSET + shakeY) * zoomScale, camRot)
	pos.x = pos.x + cam:GetTransform().position.x
	pos.y = pos.y + cam:GetTransform().position.y
	thisTransform.position = pos
	thisTransform.scale = vec3(startScaleX * zoomScale, startScaleY * zoomScale, 1)
	thisTransform.rotation = camRot

	if (Player:IsValid())
	then
    -- Modify the bar
    local MAX_HEALTH = 6
		local health = Player:GetScript("GnomeHealth.lua").health
    
    -- Set the bar - color and death.
    local spr = this:GetSprite()
    local th = spr.textureHandler
    th.currentFrame = ColorFrame(Player:GetScript("GnomeStatus.lua").isStatue)
    spr.textureHandler = th
    
    -- Set the heart frame.
    local heartSpr = hearts:GetSprite()
    local hth = heartSpr.textureHandler
    hth.currentFrame = health
    heartSpr.textureHandler = hth
    
    local color = spr.color
    color.x = 1
    color.y = 1
    color.z = 1
    color.w = 1
    spr.color = color
	end
  
  UpdateHeartsPos()
	
	hearts:GetTransform().rotation = camRot
end

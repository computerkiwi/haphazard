--[[
FILE: Level1GUI.lua
PRIMARY AUTHOR: Max Rauffer

Copyright (c) 2017 DigiPen (USA) Corporation.
]]



PlayerName = "Player1"
Player = nil

cam = nil

offset = vec2(0,0)
startScaleX = 1
startScaleY = 1

XOffset = 0
YOffset = 0

shakeAmount = 0
SHAKE_SCALE = 0.2
SHAKE_FALLOFF = 0.3

function Shake(amount)
	shakeAmount = shakeAmount + amount
end

function Start()
	cam = GameObject.FindByName("Main Camera")
  --[[ Changed to specific scale to avoid UI getting larger whenever
       I zoom out, edit a level, save, and load. -- Lya ]]
  if(this:GetName() == "GemCounter")
  then
    startScaleX = 1.5
    startScaleY = 0.5
  end

	--offset = this:GetTransform().position
end

function LateUpdate(dt)

	Player = GameObject.FindByName(PlayerName)

	-- Change shake amount
	shakeAmount = math.max(math.lerp(shakeAmount, 0 , SHAKE_FALLOFF), 0)
	
	-- Calculate shake
	local shakeAngle = math.random() * math.rad(360)
	local shakeX = math.cos(shakeAngle) * shakeAmount * SHAKE_SCALE
	local shakeY = math.sin(shakeAngle) * shakeAmount * SHAKE_SCALE
	
	local zoomScale = cam:GetCamera().zoom / 5
	this:GetTransform().position = vec2(cam:GetTransform().position.x + (XOffset + shakeX) * zoomScale, cam:GetTransform().position.y + (YOffset + shakeY) * zoomScale)
	this:GetTransform().scale = vec3(startScaleX * zoomScale, startScaleY * zoomScale, 1)

	if (Player:IsValid())
	then
		local sprite = this:GetSprite()
		local th = sprite.textureHandler
		th.currentFrame = Player:GetScript("GnomeHealth.lua").health + 2
		sprite.textureHandler = th
	end

end

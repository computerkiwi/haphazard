--[[
FILE: FadeAndChangeLevel.lua
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

HAPHAZARD_ID = 2112077054
DIGIPEN_ID   = 3440128679
CONTROLLERS_ID = Resource.FilenameToID("ControllersRecommended.png")
MENU_ID      = 3140605471

LEVEL = "MainMenu.json" -- What level to switch to?

-- Starts faded to black.
VISIBLE_DIGIPEN      = 0.75 + 0
FADE_START_DIGIPEN   = 2.0  + VISIBLE_DIGIPEN
END_DIGIPEN          = 0.5  + FADE_START_DIGIPEN
-- Switch to controllers
VISIBLE_CONTROLLERS    = 0.75 + END_DIGIPEN
FADE_START_CONTROLLERS = 1.5  + VISIBLE_CONTROLLERS
END_CONTROLLERS        = 0.5  + FADE_START_CONTROLLERS
-- Switch to Haphazard
VISIBLE_HAPHAZARD    = 0.75 + END_CONTROLLERS
FADE_START_HAPHAZARD = 1.5  + VISIBLE_HAPHAZARD
END_HAPHAZARD        = 0.5  + FADE_START_HAPHAZARD
-- Switch to Menu
VISIBLE_MENU         = 0.75 + END_HAPHAZARD
FADE_START_MENU      = 9999 + VISIBLE_MENU
END_MENU             = 0.5  + FADE_START_MENU

timer = 0

currentID = DIGIPEN_ID

function Start()

	this:GetSprite().id = currentID

end

function ContinuePressed()
	-- Spacebar
	if (OnPress(KEY.Space))
	then
		return true
	end
	
	-- A Button on the gamepads
	for player = 0,3
	do
		local A_BUTTON = 0
		if (GamepadOnPress(player, A_BUTTON))
		then
			return true
		end
	end
	
	-- Failed to find a continue.
	return false
end

function TrySwitchSprite(id)
	if (id == currentID)
	then
		return
	end
	
	this:GetSprite().id = id
	currentID = id
end

-- Maps a value from between a and b to between 0 and 1
function InverseLerp(a, b, val)
	return (val - a) / (b - a)
end

function Update(dt)
	
	local value = 0 -- Black at 0, full texture at 1
	
	
	-- Calculate the sprite's color based on where we are.
	if (timer < VISIBLE_DIGIPEN)
	then
		value = InverseLerp(0, VISIBLE_DIGIPEN, timer)
	elseif(timer < FADE_START_DIGIPEN)
	then
		value = 1
		if (ContinuePressed()) then timer = FADE_START_DIGIPEN end
	elseif(timer < END_DIGIPEN)
	then
		value = InverseLerp(END_DIGIPEN, FADE_START_DIGIPEN , timer)
		
	elseif(timer < VISIBLE_CONTROLLERS)
	then
		TrySwitchSprite(CONTROLLERS_ID)
		value = InverseLerp(END_DIGIPEN, VISIBLE_CONTROLLERS, timer)
	elseif(timer < FADE_START_CONTROLLERS)
	then
		value = 1
	elseif(timer < END_CONTROLLERS)
	then
		value = InverseLerp(END_CONTROLLERS, FADE_START_CONTROLLERS, timer)
  elseif(timer < VISIBLE_HAPHAZARD)
	then
		TrySwitchSprite(HAPHAZARD_ID)
		value = InverseLerp(END_CONTROLLERS, VISIBLE_HAPHAZARD, timer)
		-- Skip to the fade out if we push the button.
		if (ContinuePressed()) then timer = FADE_START_HAPHAZARD + value * (END_HAPHAZARD - FADE_START_HAPHAZARD) end
	elseif(timer < FADE_START_HAPHAZARD)
	then
		value = 1
		if (ContinuePressed()) then timer = FADE_START_HAPHAZARD end
	elseif(timer < END_HAPHAZARD)
	then
		value = InverseLerp(END_HAPHAZARD, FADE_START_HAPHAZARD, timer)
	elseif(timer < VISIBLE_MENU)
	then
		TrySwitchSprite(MENU_ID)
		value = InverseLerp(END_HAPHAZARD, VISIBLE_MENU, timer)
	elseif(timer < FADE_START_MENU)  -- Menu is showing
	then
		value = 1
		
		-- Hold here until we manually continue.
		timer = FADE_START_MENU - 50
		
		if (ContinuePressed())
		then
			-- Start fading out.
			timer = FADE_START_MENU
			PlaySound("button_confirm.mp3", 1, 1, false)
		end
	elseif(timer < END_MENU)
	then
		value = InverseLerp(END_MENU, FADE_START_MENU, timer)
	else -- We got through everything. Move on.
		Engine.LoadLevel(LEVEL)
	end
	
	-- Get the sprite.
	local sprite = this:GetSprite()
	-- Change the sprite's color.
	local temp = sprite.color
	temp.x = value
	temp.y = value
	temp.z = value
	sprite.color = temp
	
	-- Update the timer.
	timer = timer + dt
end

--[[
FILE: FadeAndChangeLevel.lua
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

HAPHAZARD_ID = 2112077054
DIGIPEN_ID   = 3440128679
MENU_ID      = 3140605471

LEVEL = "MainMenu.json" -- What level to switch to?

-- Starts faded to black.
VISIBLE_DIGIPEN      = 1.0 + 0
FADE_START_DIGIPEN   = 1.5 + VISIBLE_DIGIPEN
END_DIGIPEN          = 1.0 + FADE_START_DIGIPEN
-- Switch to Haphazard
VISIBLE_HAPHAZARD    = 1.0 + END_DIGIPEN
FADE_START_HAPHAZARD = 1.5 + VISIBLE_HAPHAZARD
END_HAPHAZARD        = 1.0 + FADE_START_HAPHAZARD
-- Switch to Menu
VISIBLE_MENU         = 1.0 + END_HAPHAZARD

timer = 0

CONTINUE_BUTTONS = {KEY.Space, KEY.Escape}

currentID = DIGIPEN_ID

function Start()

	this:GetSprite().id = currentID

end

function ContinuePressed()

	-- Check each button.
	for i,key in ipairs(CONTINUE_BUTTONS)
	do
		if (OnPress(key))
		then
			return true
		end
	end
	
	-- If we got here nothing was pushed.
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
		-- Skip to the fade out if we push the button.
		if (ContinuePressed()) then timer = FADE_START_DIGIPEN + value * (END_DIGIPEN - FADE_START_DIGIPEN) end
	elseif(timer < FADE_START_DIGIPEN)
	then
		value = 1
		if (ContinuePressed()) then timer = FADE_START_DIGIPEN end
	elseif(timer < END_DIGIPEN)
	then
		value = InverseLerp(END_DIGIPEN, FADE_START_DIGIPEN , timer)
		
	elseif(timer < VISIBLE_HAPHAZARD)
	then
		TrySwitchSprite(HAPHAZARD_ID)
		value = InverseLerp(END_DIGIPEN, VISIBLE_HAPHAZARD, timer)
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
	else  -- Menu is showing
		value = 1
		
		if (ContinuePressed())
		then
			PlaySound("button_confirm.mp3", 1, 1, false)
			Engine.LoadLevel(LEVEL)
		end
		
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

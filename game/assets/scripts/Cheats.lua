--[[
FILE: Cheats.lua
PRIMARY AUTHOR: Brett Schiff

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

_G.CHEAT_MODE = false
_G.GOD_MODE = false
CHEAT_ACTIVATION_KEY = "HEATER"
KEY_LENGTH = #CHEAT_ACTIVATION_KEY + 1
CHEAT_ACTIVATION_INDEX = 1

forgivenessTime = .5
currTime = 0

muteKey = KEY.M
godModeKey = KEY.G

function UpdateCheats(dt)

	if(_G.CHEAT_MODE)
	then
		
		-- if control is held
		if(IsHeld(KEY.LeftControl))
		then
			
			-- mute music
			if(OnPress(muteKey))
			then

				ToggleMusic()

			end

			-- God Mode
			if(OnPress(godModeKey))
			then

				_G.GOD_MODE = not _G.GOD_MODE

			end

		end

	end

end

function Update(dt)

	currTime = currTime + dt

	if(currTime >= forgivenessTime)
	then

		CHEAT_ACTIVATION_INDEX = 1

	end

	-- find the current key to be pressed
	local currChar = CHEAT_ACTIVATION_KEY:sub(CHEAT_ACTIVATION_INDEX, CHEAT_ACTIVATION_INDEX)
	local currKey = KEY[currChar]

	if(IsHeld(currKey))
	then

		currTime = 0
		CHEAT_ACTIVATION_INDEX = CHEAT_ACTIVATION_INDEX + 1

	end


	if(CHEAT_ACTIVATION_INDEX >= KEY_LENGTH)
	then

		_G.CHEAT_MODE = not _G.CHEAT_MODE
		_G.GOD_MODE = false
		CHEAT_ACTIVATION_INDEX = 1

	end

	UpdateCheats(dt)

end

function PausedUpdate(dt)
	
	Update(dt)

end

--[[
FILE: Cheats.lua
PRIMARY AUTHOR: Brett Schiff

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

_G.CHEAT_MODE = _G.CHEAT_MODE or false
_G.GOD_MODE = _G.GOD_MODE or false
CHEAT_ACTIVATION_KEY = "HEATER"
KEY_LENGTH = #CHEAT_ACTIVATION_KEY + 1
CHEAT_ACTIVATION_INDEX = 1

forgivenessTime = .5
currTime = 0

muteKey = KEY.M
godModeKey = KEY.G

function DisplayCheatNotification(prefabName)

	local obj = GameObject.LoadPrefab(prefabName)

	if (obj:IsValid())
	then

		local camera = GameObject.FindByName("Main Camera")
		obj:GetTransform().position = camera:GetTransform().position
		obj:GetTransform().parent = camera
	
		return obj

	else

		return nil
	
	end

end

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

				if(_G.GOD_MODE)
				then

					DisplayCheatNotification("assets/prefabs/cheats_internal/GodModeOn.json")

				else

					DisplayCheatNotification("assets/prefabs/cheats_internal/GodModeOff.json")

				end

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

		if(_G.CHEAT_MODE)
		then

			DisplayCheatNotification("assets/prefabs/cheats_internal/CheatModeOn.json")

		else

			DisplayCheatNotification("assets/prefabs/cheats_internal/CheatModeOff.json")

		end

	end

	UpdateCheats(dt)

end

function PausedUpdate(dt)
	
	Update(dt)

end

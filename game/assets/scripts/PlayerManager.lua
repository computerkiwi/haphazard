--[[
FILE: PlayerManager.lua
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

-- No start/update etc. Everything will be set up when the script is first loaded.
-- Just attach an instance of the script somewhere, probably on the intro screen.

local GNOME_PREFAB_PATH = "assets/prefabs/PlayerGnome.json"

function ConstructPlayer(playerID, controllerID, gnomeID)
	local player = {}
	
	player.playerID = playerID
	player.controllerID = controllerID
	player.gnomeID = gnomeID
	
	player.gameObject = GameObject(0)
	
	return player
end

-- Singleton pattern: There should only ever be one player manager.
if (_G.PLAYER_MANAGER == nil)
then

-- Set up the player manager table.
_G.PLAYER_MANAGER = {}
local PM = PLAYER_MANAGER

-- IDs for gnomes.
PM.GNOME_IDS = {RED = 1, GREEN = 2, BLUE = 3, YELLOW = 4}

-- Player list starts empty.
PM.PLAYERS = {}

-- TEMPORARY: AUTOMATICALLY ADD SOME PLAYERS
for i = 1,4 do
	PM.PLAYERS[#PM.PLAYERS + 1] = ConstructPlayer(#PM.PLAYERS + 1, #PM.PLAYERS, #PM.PLAYERS + 1)
end
-- END TEMPORARY

function PM.PlayerCount()
	local count = 0
	
	for k,v in pairs(PM.PLAYERS)
	do
		count = count + 1
	end
	
	return count
end

function PM.GetByPlayerID(playerID)
	return PM.PLAYERS[playerID]
end

function PM.GetByControllerID(controllerID)
	for k,v in pairs(PM.PLAYERS)
	do
		if (v.controllerID == controllerID)
		then
			return v
		end
	end
	
	-- If we got here there's no player with the id.
	return nil
end

-- Returns nil if it fails to spawn a character.
function PM.SpawnPlayerCharacter(playerID)
	-- Get the player object.
	local player = PM.PLAYERS[playerID]
	if(player == nil)
	then
		return nil
	end

	-- Spawn the actual object.
	local obj = GameObject.LoadPrefab(GNOME_PREFAB_PATH)
	if (not obj:IsValid())
	then
		return nil
	end
	
	-- Store the player for later use.
	player.gameObject = obj
	
	-- Set info on the gnome status.
	local status = obj:GetScript("GnomeStatus.lua")
	if (status ~= nil)
	then
		status.SetInfo(player.playerID, player.gnomeID)
	end

	local input = obj:GetScript("InputHandler.lua")
	if (input ~= nil)
	then
		input.PLAYER_INPUT_NUM = player.controllerID
		input.SetKeyboardControls(player.playerID)
	end
	
	return obj
end

function PM.SpawnAllPlayers()
	local playerList = {}
	-- Try to spawn a character for each player.
	for k,v in pairs(PM.PLAYERS)
	do
		local spawnedChar = PM.SpawnPlayerCharacter(k)
		if (spawnedChar ~= nil)
		then
			playerList[#playerList + 1] = spawnedChar
		end
	end
	
	return playerList
end


end

--[[
FILE: PlayMusicOnCreate.lua
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2017 DigiPen (USA) Corporation.
]]
VOLUME = 0.4

SONG_NAME = "level_1.mp3"

function Start()
	-- Temporary: play the one song we have
	-- TODO: Go through and swap out the correct files.
	local SONG_NAME = "level_1.mp3"
	
	if (GetMusic():GetFileName() ~= SONG_NAME)
	then
		PlayMusic(SONG_NAME, VOLUME, 1, 0)
	end
	
end

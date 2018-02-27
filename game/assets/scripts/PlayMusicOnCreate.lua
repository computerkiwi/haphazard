--[[
FILE: PlayMusicOnCreate.lua
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2017 DigiPen (USA) Corporation.
]]
started = false

VOLUME = 0.4

SONG_NAME = "level_1.mp3"

function Start()

	if (_G.currentMusic == nil or _G.currentMusic:GetFileName() ~= SONG_NAME)
	then
		if (_G.currentMusic ~= nil)
		then
			_G.currentMusic:Stop()
		end

		-- Temporary: play the one song we have
		_G.currentMusic = PlaySound("level_1.mp3", VOLUME, 1, true)
	end
end

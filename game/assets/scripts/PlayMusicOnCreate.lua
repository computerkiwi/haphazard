started = false

VOLUME = 0.4

SONG_NAME = "level_1.wav"

function Start()
	if (_G.currentMusic == nil or _G.currentMusic:GetFileName() ~= SONG_NAME)
	then
		_G.currentMusic = PlaySound(SONG_NAME, VOLUME, 1, true)
	end
end

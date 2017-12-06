started = false

VOLUME = 0.4

SONG_NAME = "level_1.mp3"

function Start()
	if (this:GetName() == "Screen")
	then
		SONG_NAME = "main_menu.mp3"
	end

	if (_G.currentMusic == nil or _G.currentMusic:GetFileName() ~= SONG_NAME)
	then
		if (_G.currentMusic ~= nil)
		then
			_G.currentMusic:Stop()
		end

		_G.currentMusic = PlaySound(SONG_NAME, VOLUME, 1, true)
	end
end

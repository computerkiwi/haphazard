started = false

VOLUME = 0.4

SONG_NAME = "level_1.wav"

function Start()
	PlaySound(SONG_NAME, VOLUME, 1, true)
end

function Update(dt)
	if (not started)
	then
		started = true
		Start()
	end
	
end

LEVEL = "Level1.json" -- What level to switch to?
FADE_TIME = 1.2       -- How long before level change?
COLOR_TIME = 1        -- How long before we're faded out?

timer = 0

function Update(dt)
	timer = timer + dt

	
	-- Get the sprite.
	local sprite = this:GetSprite()
	
	-- Change the sprite's color.
	local temp = sprite.color
	local value = math.max(0, (COLOR_TIME - timer) / FADE_TIME)
	temp.x = value
	temp.y = value
	temp.z = value
	sprite.color = temp
	
	if (timer > FADE_TIME)
	then
		Engine.LoadLevel(LEVEL)
	end
	
end

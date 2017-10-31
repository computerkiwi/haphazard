
SPRITE_SWITCH_TIME = 0.4

timer = 0
spriteTimer = 0

usingSprite1 = false
SPRITE_ID_1 = 80994
SPRITE_ID_2 = 154614

speed = 2

function dumpTable(tab)
	for k,v in pairs(tab) do print(tostring(k) .. " - " .. type(v)) end
end

dumpTable(_G)

function handleSprite(gameObject, dt)
	spriteTimer = spriteTimer + dt
	
	if (spriteTimer >= SPRITE_SWITCH_TIME)
	then
		print("doin it")
		
		-- Get the object and reset the timer
		local sprite = gameObject:GetSprite()
		spriteTimer = spriteTimer - SPRITE_SWITCH_TIME
		
		-- Setup the sprite.
		if (usingSprite1)
		then
			sprite.resourceID = SPRITE_ID_2
		else
			sprite.resourceID = SPRITE_ID_1
		end
		usingSprite1 = not usingSprite1
		
	end
end

function update(gameObject, dt)
	timer = timer + dt
		
	local rBody = gameObject:GetRigidBody()
	local transform = gameObject:GetTransform()
	--local transform = GameObject.GetTransform(gameObject)
	tempVel = rBody.velocity
	if (IsPressed(68)) -- D
	then
		tempVel.x = speed
	elseif(IsPressed(65)) -- A
	then
		tempVel.x = -speed
	else
		tempVel.x = 0
	end
	
	if(IsPressed(66)) -- B
	then
		local test = transform.position
		test.x = 0
		test.y = 0
		transform.position = test
	end
	
	handleSprite(gameObject, dt)
	
	rBody.velocity = tempVel
end

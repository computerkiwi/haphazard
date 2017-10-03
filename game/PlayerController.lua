
timer = 0

speed = 0.05

jumpSpeed = 0.03

function dumpTable(tab)
	for k,v in pairs(tab) do print(tostring(k) .. " - " .. type(v)) end
end

function update(gameObject, dt)
	timer = timer + dt
		
	local rBody = GameObject.GetRigidBody(gameObject)
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
	
	if (IsPressed(87)) -- W
	then
		tempVel.y = jumpSpeed
	end
	
	rBody.velocity = tempVel
end

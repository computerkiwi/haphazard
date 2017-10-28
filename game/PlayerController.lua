
timer = 0

speed = 25

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
	
	rBody.velocity = tempVel
end

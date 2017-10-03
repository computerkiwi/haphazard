
hasRun = false

xVel = -.02
yVel = 0

function update(gameObject, dt)

	local rBody = GameObject.GetRigidBody(gameObject)
	
	if (hasRun)
	then
		-- Do nothing
	else
		tempVel = rBody.velocity
		tempVel.x = 0
		tempVel.y = 0
		if (IsPressed(66)) -- B
		then
			tempVel.x = xVel
			tempVel.y = yVel
			hasRun = true
		end
		rBody.velocity = tempVel
	end
end

TIME_TO_DELETE = 2

timer = 0

function Update(dt)
	timer = timer + dt
	
	if (timer > TIME_TO_DELETE)
	then
		this:Destroy()
	end
end
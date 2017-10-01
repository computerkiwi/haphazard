
timer = 0

wobbleHeight = 0.4
wobbleSpeed  = 5

function dumpTable(tab)
	for k,v in pairs(tab) do print(tostring(k) .. " - " .. type(v)) end
end

function update(gameObject, dt)
	timer = timer + dt
	
	transform = GameObject.GetTransform(gameObject)
	local tempPosition = transform.position;
	
	tempPosition.x = tempPosition.x + dt
	tempPosition.y = math.sin(timer * wobbleSpeed) * wobbleHeight
	
	transform.position = tempPosition
end
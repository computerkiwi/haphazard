--[[
FILE: FireBulletAtMouse.lua
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2017 DigiPen (USA) Corporation.
]]
BULLET_NAME = "bullet.json"

BULLET_SPEED = 5

function Magnitude(vector)
	return math.sqrt(vector.x * vector.x + vector.y * vector.y)
end


function Normalize(vector)
	local iMag = 1 / Magnitude(vector)
	Scale(vector, iMag)
	
	return vector
end

function Scale(vector, scalar)
	vector.x = vector.x * scalar
	vector.y = vector.y * scalar

end

function Update(dt)
	if (IsTriggered(1)) --Mouse_2
	then
		local bullet = GameObject.LoadPrefab(BULLET_NAME)
		local bTransform = bullet:GetTransform()
		local bRbody = bullet:GetRigidBody()
		
		local thisPos = this:GetTransform().position
		
		local diff = vec2.Sub(GetMousePos(), thisPos)
		Normalize(diff)
		Scale(diff, BULLET_SPEED)
	
		local rotation = math.atan2(diff.y, diff.x)
		bTransform.rotation = math.deg(rotation)
	
		bTransform.position = thisPos
		
		local tempVec = bRbody.velocity
		tempVec.x = diff.x
		tempVec.y = diff.y
		bRbody.velocity = tempVec
		
		
	end
end
--[[
FILE: RangedBoi.lua
PRIMARY AUTHOR: Brett Schiff

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

-- 
PLAYER1LAYER = 1 << 7
PLAYER2LAYER = 1 << 8
PLAYER3LAYER = 1 << 9
PLAYER4LAYER = 1 << 10
GROUND_LAYER = 1 << 3

-- info about his vision
lookDirectionDegrees = 180
lookConeWidth = 45
lookConeLength = 5
lookNumRays = 4

-- info about shooting
firstShotDelay = 0
secondShotDelay = .2
thirdShotDelay = .2
interFiringPatternDelay = 1

function LookForGnomes()

	-- start downward and adjust according to the cone width
	local initialDirection = lookDirectionDegrees + (lookConeWidth / 2)
	local directionIncrease = lookConeWidth / (lookNumRays - 1)
	local endDirection = lookDirectionDegrees - (lookConeWidth / 2)

	-- raycast a number of times in the correct directions
	for currDirection = initialDirection, endDirection, directionIncrease
	do
		 -- raycast from the bird
		 local cast = Raycast.RaycastAngle(this:GetSpaceIndex(), this:GetTransform().position, currDirection, lookConeLength, PLAYER1LAYER | PLAYER2LAYER | PLAYER3LAYER | PLAYER4LAYER | GROUND_LAYER)

		 if(cast.gameObjectHit:IsValid() and cast.gameObjectHit:GetCollider().collisionLayer.layer ~= GROUND_LAYER)
		 then

			return cast.gameObjectHit

		 end

	end

end

function ShootAtTarget()

	

end


-- Called at initialization
function Start()
	
	

end

-- Updates each frame
function Update(dt)
	
	-- look for gnomes
	local targetGnome = LookForGnomes()

	-- shoot at the target gnomes
	ShootAtTarget(targetGnome)

end

-- Other is a game object
function OnCollisionEnter(other)

	

end

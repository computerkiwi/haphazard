--[[
FILE: InteractiveEnvironentMovement.lua
PRIMARY AUTHOR: Ash Duong

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

InteractiveMachineName = ""
InteractiveMachine = nil
Start_X = 0
Start_Y = 0
End_X = 0
End_Y = 0
percent = 0

function Update(dt)
	InteractiveMachine = GameObject.FindByName(InteractiveMachineName)

    local transform = this:GetTransform()
    local currPos = transform.position
    local newPos = transform.position

	if (InteractiveMachine:IsValid())
	then
		if (InteractiveMachine:GetScript("InteractiveMachine.lua").Activated == true)
		then
		    newPos.x = currPos.x + (percent * (End_X - currPos.x))
		    newPos.y = currPos.y + (percent * (End_Y - currPos.y))

			transform.position = newPos
		elseif (InteractiveMachine:GetScript("InteractiveMachine.lua").Activated == false)
		then
			newPos.x = currPos.x + (percent * (Start_X - currPos.x))
		    newPos.y = currPos.y + (percent * (Start_Y - currPos.y))

			transform.position = newPos
		end
	end
end

--[[
FILE: CheatTeleport.lua
PRIMARY AUTHOR: Brett Schiff

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

-- Updates each frame
function Update(dt)

	if(_G.CHEAT_MODE and IsHeld(KEY.LeftControl))
	then
	
		local obj1 = GameObject.FindByName("CheatTeleport1")
		local obj2 = GameObject.FindByName("CheatTeleport2")
		local obj3 = GameObject.FindByName("CheatTeleport3")
		local obj4 = GameObject.FindByName("CheatTeleport4")
		local obj5 = GameObject.FindByName("CheatTeleport5")

		if(OnPress(KEY.One) and obj1:IsValid())
		then

			local location = obj1:GetTransform().position
			this:GetTransform().position = location

		end

		if(OnPress(KEY.Two) and obj2:IsValid())
		then

			local location = obj2:GetTransform().position
			this:GetTransform().position = location

		end

		if(OnPress(KEY.Three) and obj3:IsValid())
		then

			local location = obj3:GetTransform().position
			this:GetTransform().position = location

		end

		if(OnPress(KEY.Four) and obj4:IsValid())
		then

			local location = obj4:GetTransform().position
			this:GetTransform().position = location

		end

		if(OnPress(KEY.Five) and obj5:IsValid())
		then

			local location = obj5:GetTransform().position
			this:GetTransform().position = location

		end

	end

end -- fn end

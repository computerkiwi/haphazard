--[[
FILE: DangerShroom.lua
PRIMARY AUTHOR: Noah Sweet

Copyright (c) 2017 DigiPen (USA) Corporation.
]]



function Start()
end -- fn end


function Update(dt)
end -- fn end


function OnCollisionEnter(object)
	if object:HasTag("Player")
	then
		
	else
		this:Destroy();
	end
end -- fn end


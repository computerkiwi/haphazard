--[[
FILE: Door.lua
PRIMARY AUTHOR: Ash Duong

Copyright (c) 2017 DigiPen (USA) Corporation.
]]--

maxKilledGnomes = 2
local killedGnomes = 0

function OnCollisionEnter(other)

	if(other:HasTag("Player") and killedGnomes < maxKilledGnomes)
	then
		
		other:GetScript("GnomeHealth.lua").Damage(1000, nil)

		killedGnomes = killedGnomes + 1

	end

	if(killedGnomes >= maxKilledGnomes)
	then

		this:Destroy()

	end

end

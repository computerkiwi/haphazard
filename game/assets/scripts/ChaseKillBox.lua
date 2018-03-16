--[[
FILE: ChaseKillBox.lua
PRIMARY AUTHOR: Max

Copyright (c) 2017 DigiPen (USA) Corporation.
]]

Section1_Start = 0
Section1_Speed = 0.1

Section2_Y_Start = 0
Section2_Speed = 0.1

Section3_Y_Start = 0
Section3_Speed = 0.1

Section4_Y_Start = 0
Section4_Speed = 0.1

Section5_Y_Start = 0
Section5_Speed = 0.1

Section6_Y_Start = 0
Section6_Speed = 0.1

Section7_Y_Start = 0
Section7_Speed = 0.1

Section8_Y_Start = 0
Section8_Speed = 0.1

currSection = 1
currSpeed = 0.1

function Start()
	currSection = 1;
	currSpeed = Section1_Speed;
	this:GetTransform().position.y = Section1_Start;
end

function Update(dt)

	this:GetTransform().position.y = this:GetTransform().position.y + currSpeed * dt;

	-- This is gross but the only way :(

	if(currSection == 1)
	then
		if(this:GetTransform().position.y > Section2_Y_Start)
		then
			currSection = 2;
			currSpeed = Section2_Speed;
		end


	elseif(currSection == 2)
	then
		if(this:GetTransform().position.y > Section3_Y_Start)
		then
			currSection = 3;
			currSpeed = Section3_Speed;
		end


	elseif(currSection == 3)
	then
		if(this:GetTransform().position.y > Section4_Y_Start)
		then
			currSection = 4;
			currSpeed = Section4_Speed;
		end


	elseif(currSection == 4)
	then
		if(this:GetTransform().position.y > Section5_Y_Start)
		then
			currSection = 5;
			currSpeed = Section5_Speed;
		end


	elseif(currSection == 5)
	then
		if(this:GetTransform().position.y > Section6_Y_Start)
		then
			currSection = 6;
			currSpeed = Section6_Speed;
		end


	elseif(currSection == 6)
	then
		if(this:GetTransform().position.y > Section7_Y_Start)
		then
			currSection = 7;
			currSpeed = Section7_Speed;
		end


	elseif(currSection == 7)
	then
		if(this:GetTransform().position.y > Section8_Y_Start)
		then
			currSection = 8;
			currSpeed = Section8_Speed;
		end
	end
		
end
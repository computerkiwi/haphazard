--[[
FILE: Looker.lua
PRIMARY AUTHOR: Kieran Williams

Copyright (c) 2018 DigiPen (USA) Corporation.
]]

lerpSpeed = 0.2
eyesRadius = 0.032

eyesObj = nil

function lerp(a, b, t)
	return a + t * (b - a)
end

function Length(vec)
	return math.sqrt(vec.x * vec.x + vec.y * vec.y)
end

function Normalized(vec)
	local length = Length(vec)
	return vec2(vec.x / length, vec.y / length)
end

function Multiply(scalar, vec)
	return vec2(vec.x * scalar, vec.y * scalar)
end

function FindEyes()
	local eyesName = this:GetName() .. "_Eyes"
	
	eyesObj = GameObject.FindByName(eyesName)
	if (not eyesObj:IsValid())
	then
		eyesObj = nil
		return
	end
	
	local transform = eyesObj:GetTransform()
	transform.parent = this
	transform.localPosition = vec2(0, 0)
end

-- Returns the gnome to look at.
function GetLookPos()
	local player = GameObject.FindByName("Player1")
	if (player:IsValid())
	then
		return player:GetTransform().position
	else
		return vec2(0,0)
	end
	
	
end

function Start()
	FindEyes()
end


function Update(dt)
	if (eyesObj == nil)
	then
		FindEyes()
		return
	end

	local eyesTransform = eyesObj:GetTransform()
	local thisTransform = this:GetTransform()
	
	
	local lookPos = GetLookPos()
	local offset = vec2.Sub(lookPos, thisTransform.position)
	offset = Normalized(offset)
	offset = Multiply(eyesRadius, offset)
	
	local finalLocalPos = vec2(lerp(eyesTransform.localPosition.x, offset.x, lerpSpeed), lerp(eyesTransform.localPosition.y, offset.y, lerpSpeed))
	eyesTransform.localPosition = finalLocalPos	
end


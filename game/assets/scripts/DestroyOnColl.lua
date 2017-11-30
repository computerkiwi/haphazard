function Update(dt)

end

function QuoteUnquoteKill(object)
	local transform = object:GetTransform()
	local tempVec = transform.position
	tempVec.x = 9999999 + math.random(1000)
	tempVec.y = 9999999 + math.random(1000)
	transform.position = tempVec
end

function OnCollisionEnter(other)
  
	QuoteUnquoteKill(this)
	QuoteUnquoteKill(other)
	
end -- fn end

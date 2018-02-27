function TestMessageFunction(a, b, c, other)
	print("Receiving message " .. tostring(a) .. ", " .. tostring(b) .. ", " .. tostring(c) .. " from " .. tostring(other:GetName()))
end
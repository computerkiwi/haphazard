if (someVar ~= nil)
then
	print("someVar is not nil!")
else
	print("someVar is nil. This is good.")
end

if (_G.someVar ~= nil)
then
	print("_G.someVar is " .. _G.someVar)
else
	print("_G.someVar is nil.")
end

someVar = 5

_G.someVar = 6

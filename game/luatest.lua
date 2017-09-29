print("Welcome to Lua.")

function DumpTableTypes(tab)
	for i, v in pairs(tab) do print(i .. ' - ' .. type(v)) end

end

DumpTableTypes(_G)

print("-------------------------------------------------------------")

print(type(TestA))
DumpTableTypes(getmetatable(TestA))

someThing = 5

myTable = {}

for i = 1,10
do
	myTable[i] = i * i
end

myTable["fuck"] = 100

print(myTable[1])

print(myTable["fuck"])
print(myTable.fuck)

myTable.func = function() print("I'm a function!") end

myTable.func()
myTable["func"]()

function multipleReturn(num)
	return num, num * num, num * num * num, num * num * num * num
end

anotherTable = {multipleReturn(10)}

for i,v in ipairs(anotherTable) do print(i .. ', ' .. v) end

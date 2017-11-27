-- Simple test for getting scripts from within scripts.

function dumpTable(tab)
	for k,v in pairs(tab) do print(tostring(k) .. " - " .. type(v)) end
end

function Update(dt)
	local script = this:GetScript("PlayerController.lua")
	
	--dumpTable(scriptTable)
	
	script.STACK_HEIGHT = 50
	
end
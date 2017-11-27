function dumpTable(tab)
	for k,v in pairs(tab) do print(tostring(k) .. " - " .. type(v)) end
end

function Update(dt)
	local scripts = this:GetScripts()
	local script = scripts:GetScriptByFilename("PlayerController.lua")
	local scriptTable = script:GetScriptEnvironment()

	dumpTable(scriptTable)
	
	scriptTable.STACK_HEIGHT = 50
	
end
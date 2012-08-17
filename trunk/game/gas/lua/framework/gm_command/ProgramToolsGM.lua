
local 	nFileNum = 0

function PrintLuaMemory(threadName,maxLevel)
	local tid = GetCurrentThreadID()	
	nFileNum = nFileNum + 1
	local fileName = string.format("LuaObjMem%d_%d(%s).log", tid, nFileNum, threadName)
	
	if threadName == "logic" then
		CheckAllObjLeak(fileName, maxLevel)
	elseif string.sub(threadName, 1, 2) == "vm" then
		local vmId = tonumber(string.sub(threadName, 3, -1))
		if vmId and vmId >= 0 and vmId <= g_ChildVMNum then
			local data = {}
			data["maxLevel"] = maxLevel
			data["fileName"] = fileName
			CallSpecialVMDbTrans(vmId, "GMDB", "PrintMemoryData", nil, nil, data)
		else
			local msg = string.format("the arg1 is  'logic', 'vm0' ~ 'vm%d'", g_ChildVMNum)
			return msg
		end
	else
		local msg = string.format("目前可以使用的线程名是: 'logic', 'vm0' ~ 'vm%d'", g_ChildVMNum)
		return msg
	end
	
	return "Done"
end
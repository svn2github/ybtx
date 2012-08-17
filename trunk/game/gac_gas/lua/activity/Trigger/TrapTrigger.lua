cfg_load "trigger/TrapTrigger_Common"

g_TrapTrigger = {}
AddCheckLeakFilterObj(g_TrapTrigger)

local function LoadTrapTrigger_Common()
	local TrapTriggerKeys = TrapTrigger_Common:GetKeys()
	if not TrapTriggerKeys then
		return
	end
	for _, TrapName in pairs(TrapTriggerKeys) do
 		if g_TrapTrigger[TrapName] == nil then
			g_TrapTrigger[TrapName] = {}
		end
		local TriggerTypeKeys = TrapTrigger_Common:GetKeys(TrapName)
		for _, TriggerType in pairs(TriggerTypeKeys) do
			if g_TrapTrigger[TrapName][TriggerType] == nil then
				g_TrapTrigger[TrapName][TriggerType] = {}
			end 
			local ScriptKeys = TrapTrigger_Common:GetKeys(TrapName, TriggerType)
			for _, Script in pairs(ScriptKeys) do
				local Arg = TrapTrigger_Common(TrapName, TriggerType, Script, "Arg")
				local data = {}
				data["TrapName"] = TrapName
				data["TriggerType"] = TriggerType
				data["Script"] = Script
				data["Arg"] = loadstring("return {" .. (Arg or "") .. "}")()
				table.insert(g_TrapTrigger[TrapName][TriggerType], data)
			end
		end
	end
end

LoadTrapTrigger_Common()
UnloadCfgTbl("TrapTrigger_Common")

function GetTrapTriggerArg(TrapName, TriggerType, Script)

	if not g_TrapTrigger[TrapName] then
		return
	end
	local result = g_TrapTrigger[TrapName]
	
	if IsString(TriggerType) then
		result = result[TriggerType]
		if not result then
			return
		end
	end
	
	if IsString(Script) then
		for i, v in pairs(result) do
			if v.Script == Script then
				result = v
			end
		end
	end
	
	return result
end

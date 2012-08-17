cfg_load "trigger/ObjTrigger_Common"

g_ObjTrigger = {}
AddCheckLeakFilterObj(g_ObjTrigger)

local function LoadObjTrigger_Common()
	local ObjTriggerKeys = ObjTrigger_Common:GetKeys()
	if not ObjTriggerKeys then
		return
	end
	for _, ObjName in pairs(ObjTriggerKeys) do
 		if g_ObjTrigger[ObjName] == nil then
			g_ObjTrigger[ObjName] = {}
		end
		local TriggerTypeKeys = ObjTrigger_Common:GetKeys(ObjName)
		for _, TriggerType in pairs(TriggerTypeKeys) do
			if g_ObjTrigger[ObjName][TriggerType] == nil then
				g_ObjTrigger[ObjName][TriggerType] = {}
			end 
			local ScriptKeys = ObjTrigger_Common:GetKeys(ObjName, TriggerType)
			for _, Script in pairs(ScriptKeys) do
				local Arg = ObjTrigger_Common(ObjName, TriggerType, Script, "Arg")
				local data = {}
				data["ObjName"] = ObjName
				data["TriggerType"] = TriggerType
				data["Script"] = Script
				data["Arg"] = loadstring("return {" .. (Arg or "") .. "}")()
				table.insert(g_ObjTrigger[ObjName][TriggerType], data)
			end
		end
	end
end

LoadObjTrigger_Common()
UnloadCfgTbl("ObjTrigger_Common")

function GetObjTriggerArg(ObjName, TriggerType, Script)

	if not g_ObjTrigger[ObjName] then
		return
	end
	local result = g_ObjTrigger[ObjName]
	
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

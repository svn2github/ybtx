cfg_load "trigger/NpcTrigger_Common"
cfg_load "trigger/TriggerCondition_Server"

--local function LoadNpcTrigger_Common()
--	local multiIndex = { } 
--	local mt = { 
--		__index = function(t, k)
--			 return rawget(t, k) or multiIndex[k]
--		end,
--		__newindex = function() 
--			error("can't not write a read-only table")
--		end,
--	}
--	for k,v in pairs(NpcTrigger_Common) do
--		rawset(v, "TriggerType", v.TriggerType)
--		rawset(v, "Script", v.Script)
--		rawset(v, "Arg", loadstring("return {" .. (v.Arg or "") .. "}")())
--		if multiIndex[v.NpcName] == nil then
--			multiIndex[v.NpcName] = { }
--		end
--		table.insert(multiIndex[v.NpcName], v)
--	end
--	setmetatable(NpcTrigger_Common, mt)
--end
--LoadNpcTrigger_Common()

g_NpcTrigger = {}
AddCheckLeakFilterObj(g_NpcTrigger)

local function LoadNpcTrigger_Common()
	local NpcTriggerKeys = NpcTrigger_Common:GetKeys()  --»ñµÃNpc
	if not NpcTriggerKeys then
		return 
	end
	for _, NpcName in pairs(NpcTriggerKeys) do
 		if g_NpcTrigger[NpcName] == nil then
			g_NpcTrigger[NpcName] = {}
		end
		local TriggerTypeKeys = NpcTrigger_Common:GetKeys(NpcName)
		for _, TriggerType in pairs(TriggerTypeKeys) do
			if g_NpcTrigger[NpcName][TriggerType] == nil then
				g_NpcTrigger[NpcName][TriggerType] = {}
			end 
			local ScriptKeys = NpcTrigger_Common:GetKeys(NpcName, TriggerType)
			for _, Script in pairs(ScriptKeys) do
				local Arg = NpcTrigger_Common(NpcName, TriggerType, Script, "Arg")
				local data = {}
				data["NpcName"] = NpcName
				data["TriggerType"] = TriggerType
				data["Script"] = Script
				data["Arg"] = loadstring("return {" .. (Arg or "") .. "}")()
				g_NpcTrigger[NpcName][TriggerType][Script] = data
			end
		end
	end
--	for k, v in pairs(NpcTrigger_Common) do
--		if g_NpcTrigger[v.NpcName] == nil then
--			g_NpcTrigger[v.NpcName] = {}
--		end
--		if g_NpcTrigger[v.NpcName][v.TriggerType] == nil then
--			g_NpcTrigger[v.NpcName][v.TriggerType] = {}
--		end
--		rawset(v, "Arg", loadstring("return {" .. (v.Arg or "") .. "}")())
--		g_NpcTrigger[v.NpcName][v.TriggerType][v.Script] = v
--	end
end

g_TriggerCondition = {}
AddCheckLeakFilterObj(g_TriggerCondition)

local function LoadTriggerCondition_Server()
	local TriggerConditionKeys = TriggerCondition_Server:GetKeys()
	if not TriggerConditionKeys then
		return 
	end
	for _, Type in pairs(TriggerConditionKeys) do
		if g_TriggerCondition[Type] == nil then
			g_TriggerCondition[Type] = {}
		end
		local NameKeys = TriggerCondition_Server:GetKeys(Type)
		for _, Name in pairs(NameKeys) do
			if g_TriggerCondition[Type][Name] == nil then
				g_TriggerCondition[Type][Name] = {}
			end
			local TriggerTypeKeys = TriggerCondition_Server:GetKeys(Type, Name)
			for _, TriggerType in pairs(TriggerTypeKeys) do
				if g_TriggerCondition[Type][Name][TriggerType] == nil then
					g_TriggerCondition[Type][Name][TriggerType] = {}
				end
				local ConditionKeys = TriggerCondition_Server:GetKeys(Type, Name, TriggerType)
				for _, Condition in pairs(ConditionKeys) do
					local Arg = TriggerCondition_Server(Type, Name, TriggerType, Condition, "Arg")
					local data = {}
					data["Type"] = Type
					data["Name"] = Name
					data["TriggerType"] = TriggerType
					data["Condition"] = Condition
					data["Arg"] = loadstring("return {" .. (Arg or "") .. "}")()
					g_TriggerCondition[Type][Name][TriggerType][Condition] = data
				end
			end
		end
	end
--	for k, v in pairs(TriggerCondition_Server) do
--		if g_TriggerCondition[v.Type] == nil then
--			g_TriggerCondition[v.Type] = {}
--		end
--		if g_TriggerCondition[v.Type][v.Name] == nil then
--			g_TriggerCondition[v.Type][v.Name] = {}
--		end
--		if g_TriggerCondition[v.Type][v.Name][v.TriggerType] == nil then
--			g_TriggerCondition[v.Type][v.Name][v.TriggerType] = {}
--		end
--		rawset(v, "Arg", loadstring("return {" .. (v.Arg or "") .. "}")())
--		g_TriggerCondition[v.Type][v.Name][v.TriggerType][v.Condition] = v
--	end
end

LoadNpcTrigger_Common()
LoadTriggerCondition_Server()

function GetNpcTriggerArg(NpcName, TriggerType, Script)

	if not g_NpcTrigger[NpcName] then
		return
	end
	local result = g_NpcTrigger[NpcName]
	
	if IsString(TriggerType) then
		result = result[TriggerType]
		if not result then
			return
		end
	end
	
	if IsString(Script) then
		result = result[Script]
	end
	
	return result
end

function GetTriggerCondition(Type, NpcName, TriggerType, Condition)
	if not g_TriggerCondition[Type] then
		return
	end
	if not g_TriggerCondition[Type][NpcName] then
		return
	end
	local result = g_TriggerCondition[Type][NpcName]
	
	if IsString(TriggerType) then
		result = result[TriggerType]
		if not result then
			return
		end
	end
	
	if IsString(Condition) then
		result = result[Condition]
	end
	
	return result
end

UnloadCfgTbl("NpcTrigger_Common")
UnloadCfgTbl("TriggerCondition_Server")

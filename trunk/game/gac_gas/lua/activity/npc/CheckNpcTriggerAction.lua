cfg_load "npc/NpcShowContent_Common"
lan_load "npc/Lan_NpcShowContent_Common"
-- 加载Npc触发表
g_NpcShowContentCfg = {}
AddCheckLeakFilterObj(g_NpcShowContentCfg)

--local function LoadNpcTriggerAction()
--	local multiIndex = { } 
--	local mt = { 
--		__index = function(t, k)
--			 return rawget(t, k) or multiIndex[k]
--		end,
--		__newindex = function() 
--			error("can't not write a read-only table")
--		end,
--	}
--	for k,v in pairs(NpcTriggerEvent_Common) do
--		rawset(v, "TriggerType", v.TriggerType)
----		ScriptName = assert(loadstring("return {" .. (ScriptName or "") .. "}"))()
----		rawset(v, "ScriptName", ScriptName)
----		rawset(v, "ScriptName", loadstring("return {" .. (v.ScriptName or "") .. "}")())
--		rawset(v, "ScriptArg", loadstring("return {" .. (v.ScriptArg or "") .. "}")())
--		rawset(v, "QuestRequire", loadstring("return {" .. (v.QuestRequire or "") .. "}")())
--		if multiIndex[v.NpcName] == nil then
--			multiIndex[v.NpcName] = { }
--		end
--		table.insert(multiIndex[v.NpcName], v)
--	end
--	setmetatable(NpcTriggerEvent_Common, mt)
--end
--LoadNpcTriggerAction()


local ReplaceStrTbl = {
	["#class#"] = true,
	["#sex#"] = true,
	["#name#"] = true,
}

local function LoadNpcShowContent()
	local multiIndex = {}
	local mt =
	{
		_index = function(t,k)
			return rawget(t, k) or multiIndex[k]
		end,
		_newindex = function()
			error("can't not write a read-only table")
		end
	}

	for _, NpcName in pairs(NpcShowContent_Common:GetKeys()) do
		if g_NpcShowContentCfg[NpcName] == nil then
			g_NpcShowContentCfg[NpcName] = {}
		end	
		for _, TriggerType in pairs(NpcShowContent_Common(NpcName):GetKeys()) do
			
			for _, Arg in pairs(NpcShowContent_Common(NpcName, TriggerType):GetKeys()) do
				local restbl = {}
				local tbl = {}
				local ContentTbl = {}
				local index = NpcName .. TriggerType
				if TriggerType == "释放技能" or TriggerType == "技能警告提示" then
					index = index .. Arg
				end
				local ShowConTbl = {
						["ShowContext"] = 0,
						["ShowContext1"] = 1,
						["ShowContext2"] = 2,
						["ShowContext3"] = 3,
					}
				local MusicConTbl = {
					[0] = "Music",
					[1] = "Music1",
					[2] = "Music2",
					[3] = "Music3",
				}
				for i, num in pairs(ShowConTbl) do
					local temp = Lan_NpcShowContent_Common(MemH64(index),i)
					local musicArg = MusicConTbl[num]
					local music = NpcShowContent_Common(NpcName, TriggerType, Arg, musicArg)
					if temp ~= "" then
						table.insert(ContentTbl,{temp, music})
					end
				end
				
				if NpcShowContent_Common(NpcName, TriggerType, Arg, "Channel") ~= "" then
					local tempChannel = NpcShowContent_Common(NpcName, TriggerType, Arg, "Channel")
					tbl["Channel"]	= loadstring("return {"..tempChannel.."}")()
				end
				
				if TriggerType == "释放技能" or TriggerType == "技能警告提示" then
					--restbl[v.Arg] = tbl
					if not g_NpcShowContentCfg[NpcName][TriggerType] then
						g_NpcShowContentCfg[NpcName][TriggerType] = {}
					end
					g_NpcShowContentCfg[NpcName][TriggerType][Arg] = tbl
					g_NpcShowContentCfg[NpcName][TriggerType][Arg]["ContentTbl"] = ContentTbl
					if TriggerType == "技能警告提示" then
						g_NpcShowContentCfg[NpcName][TriggerType][Arg]["Replace"] = {}
						for j, str in pairs(ContentTbl) do
							for replaceStr, _ in pairs(ReplaceStrTbl) do
								if string.find(str[1], replaceStr) then
									g_NpcShowContentCfg[NpcName][TriggerType][Arg]["Replace"][j] = replaceStr
									break
								end
							end
						end
					end
				else
					if TriggerType ~= "战斗中" then
						tbl["Arg"] = Arg
					else
						tbl["Arg"] = loadstring("return " .. (Arg or ""))()
					end
					restbl = tbl
					g_NpcShowContentCfg[NpcName][TriggerType] = restbl
					g_NpcShowContentCfg[NpcName][TriggerType]["ContentTbl"] = ContentTbl
				end
			end
		end
	end
	NpcShowContent_Common = nil
end

LoadNpcShowContent()

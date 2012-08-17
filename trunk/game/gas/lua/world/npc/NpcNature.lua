gas_require "world/npc/NpcNatureInc"
cfg_load "npc/NpcNature_Server"

function CNpcNature:Ctor()
	self.m_tblNature = {}
	local tblCheck = {}
	for _, i in pairs(NpcNature_Server:GetKeys()) do
		for _, j in pairs(NpcNature_Server:GetKeys(i)) do
			local info = NpcNature_Server(i, j)
			self.m_tblNature[i] = self.m_tblNature[i] or {}
		
			--检查Npc配置表中是重复配置Npc个性
			local bRepeat = self.m_tblNature[i][j]
			if bRepeat ~= nil then
				str = "【NpcNature_Server】配置表中【" .. i .. "】的个性填写错误，请查证！"
				error_print( false, str)
			end
			local tbl = {}
			tbl["NPCName"] = i
			tbl["NatureName"] = j
			tbl["NatureArg"] = info("NatureArg")
			tbl["NatureExtraArg"] = info("NatureExtraArg")
			tbl["NatureAction"] = info("NatureAction")
			tbl["ColdTime"] = info("ColdTime")
			tbl["NextToDo"] = info("NextToDo")
			tbl["EyeSize"] = info("EyeSize")
			self.m_tblNature[i][j] = tbl
		end
	end
end

function CNpcNature:HaveNature(NpcName)
	return self.m_tblNature[NpcName] ~= nil
end

function CNpcNature:InitNature(NpcID)
	local npc = CCharacterDictator_GetCharacterByID(NpcID)
	npc:SetNpcSleepState(false)
	local tblNature = self.m_tblNature[npc.m_Properties:GetCharName()]
	local AoiType = EObjectAoiType.EAoi_TaskNpc
	local EyeSize = 0
	for i, v in pairs(tblNature) do
			local NpcAoiType = g_NatureNpcAoi[v.NatureName]
			if NpcAoiType == nil then
				assert(false, "请在lua代码的g_NatureNpcAoi中配置 "..q["NatureName"].." 的Aoi类型")
			elseif NpcAoiType ~= EObjectAoiType.EAoi_TaskNpc then
				AoiType = NpcAoiType
			end
			
			local arg = v["NextToDo"]
			local ActionArg = {}
			if arg ~= "" then
				ActionArg = assert(loadstring("return " .. arg))()
			else
				ActionArg = {
								[1] = "",
								[2] = "",
								[3] = 0,
							}
			end
			if v.EyeSize ~= nil and v.EyeSize ~= "" then
				EyeSize = v.EyeSize
			end
			npc:SetNatureInfo(i, (v["NatureArg"] or "") .. "" , (v["NatureExtraArg"] or 0), (v["NatureAction"] or ""), v["ColdTime"] or 0, ActionArg[1] or "", ActionArg[2] or "", ActionArg[3] or 0)
	end
	npc:UniteObjectAoiType(AoiType)
	npc:SetSecondEyeSight(EyeSize)
end

g_NpcNature = g_NpcNature or CNpcNature:new()


cfg_load "fb_game/MercenaryActionAward_Server"
cfg_load "fb_game/MercenaryEducateAction_Server"
cfg_load "scene/YbEducateActionTransport_Server"

--解析一下配置表
g_MercenaryEduActMgr = {}
g_MercenaryAwardItemMgr = {}

local function LoadYbEducateActTransportCfg()
	g_MercenaryEduActMgr.m_RoomPoints = {}
	g_MercenaryEduActMgr.m_RoomPoints.MaxRoomNum = YbEducateActionTransport_Server:GetHeight()
	for _, i in pairs(YbEducateActionTransport_Server:GetKeys()) do
		local tbl = YbEducateActionTransport_Server(i)
		g_MercenaryEduActMgr.m_RoomPoints[i] = {}
		g_MercenaryEduActMgr.m_RoomPoints[i].TrapPoint = loadstring("return " .. (tbl("TrapPoint") or ""))()
		g_MercenaryEduActMgr.m_RoomPoints[i].DynamicPoint = loadstring("return " .. (tbl("DynamicPoint") or ""))()
		g_MercenaryEduActMgr.m_RoomPoints[i].SwitchPoint = loadstring("return " .. (tbl("SwitchPoint") or ""))()
	end
end

local function LoadMercenaryEducateActionCfg()
	for _, i in pairs(MercenaryEducateAction_Server:GetKeys()) do
		local CommonMgr = MercenaryEducateAction_Server(i)
		g_MercenaryEduActMgr[i] = {}
		g_MercenaryEduActMgr[i].m_GameID = CommonMgr("GameID")
		g_MercenaryEduActMgr[i].m_GameType = CommonMgr("GameType")
		g_MercenaryEduActMgr[i].m_MinLevel = CommonMgr("MinLevel")
		g_MercenaryEduActMgr[i].m_MinFloor = CommonMgr("MinFloor")
		g_MercenaryEduActMgr[i].m_GameCamp = CommonMgr("GameCamp")
		g_MercenaryEduActMgr[i].m_NpcFilePath = CommonMgr("NpcFilePath")
		g_MercenaryEduActMgr[i].m_BornPoint = loadstring("return " .. CommonMgr("BornPoint"))()
		g_MercenaryEduActMgr[i].m_FitoutTime = CommonMgr("FitoutTime")
		g_MercenaryEduActMgr[i].m_RulesText = CommonMgr("RulesText")
		g_MercenaryEduActMgr[i].m_OverType = CommonMgr("OverType")
		if CommonMgr("LoadSkill") ~= "" then
			g_MercenaryEduActMgr[i].m_LoadSkillTbl = loadstring("return {" .. CommonMgr("LoadSkill") .. "}")()
		end
		if CommonMgr("ResetSkill") ~= "" then
			g_MercenaryEduActMgr[i].m_ResetSkillTbl = loadstring("return {" .. CommonMgr("ResetSkill") .. "}")()
		end
		if CommonMgr("SuccLevel") ~= "" then
			g_MercenaryEduActMgr[i].m_SuccLevel = loadstring("return " .. CommonMgr("SuccLevel"))()
		end
		if CommonMgr("AwardExp") ~= "" then
			g_MercenaryEduActMgr[i].m_AwardExp = CommonMgr("AwardExp")
		end
		if CommonMgr("AwardFetch") ~= "" then
			g_MercenaryEduActMgr[i].m_AwardFetch = CommonMgr("AwardFetch")
		end
		if CommonMgr("AwardMoney") ~= "" then
			g_MercenaryEduActMgr[i].m_AwardMoney = CommonMgr("AwardMoney")
		end
		
		local OverTypeMgr = CommonMgr("OverType")
		local tblStr = string.gsub(OverTypeMgr, "}[^{]*", "},")
		local OverTypeTbl = loadstring("return {" .. tblStr .. "}")()
		g_MercenaryEduActMgr[i].OverTypeTbl = OverTypeTbl
		local overFunStr = OverTypeMgr
		for _, overType in ipairs(OverTypeTbl) do
			overFunStr = string.gsub(overFunStr, "%b{}", " arg[\"" .. overType[1] .. "\"] == true ", 1 )
		end
		overFunStr = "local function IsOver(arg)  return (" .. overFunStr .. ") end return IsOver"
		g_MercenaryEduActMgr[i].IsOverFun = loadstring(overFunStr)()
		
		local LoseGameTypeMgr = CommonMgr("LoseGameType")
		
		local tblStr = nil
		if LoseGameTypeMgr then
			tblStr = string.gsub(LoseGameTypeMgr, "}[^{]*", "},")
		end
		
		if tblStr and tblStr~="" then
			local LoseGameTypeTbl = loadstring("return {" .. tblStr .. "}")()
			g_MercenaryEduActMgr[i].LoseGameTypeTbl = LoseGameTypeTbl
			local LoseGameFunStr = LoseGameTypeMgr
			for _, LoseGameType in ipairs(LoseGameTypeTbl) do
				LoseGameFunStr = string.gsub(LoseGameFunStr, "%b{}", " arg[\"" .. LoseGameType[1] .. "\"] == true ", 1 )
			end
			LoseGameFunStr = "local function IsLoseGame(arg)  return (" .. LoseGameFunStr .. ") end return IsLoseGame"
			g_MercenaryEduActMgr[i].IsLoseGameFun = loadstring(LoseGameFunStr)()
		end
	end
end

local function LoadMercenaryActionAwardCfg()
	--for GameName,CommonMgr in pairs(MercenaryActionAward_Server) do
	for _, 	GameName in pairs(MercenaryActionAward_Server:GetKeys()) do
		local CommonMgr = MercenaryActionAward_Server(GameName)
		g_MercenaryAwardItemMgr[GameName] = {}
		for j=1, 150 do
			local strLevel = "Level"..j
			local strItem = "Item"..j
			if CommonMgr(strLevel) and CommonMgr(strItem) then
				local LevelTbl = loadstring("return "..CommonMgr(strLevel))()
				local ItemTbl = loadstring("return {"..CommonMgr(strItem).."}")()
				table.insert(g_MercenaryAwardItemMgr[GameName],{LevelTbl,ItemTbl})
			end
		end
	end
end

LoadMercenaryEducateActionCfg()
LoadMercenaryActionAwardCfg()
LoadYbEducateActTransportCfg()
UnloadCfgTbl("MercenaryEducateAction_Server")
AddCheckLeakFilterObj(g_MercenaryEduActMgr)
AddCheckLeakFilterObj(g_MercenaryAwardItemMgr)

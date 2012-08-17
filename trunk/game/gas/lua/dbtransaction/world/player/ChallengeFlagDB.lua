cfg_load "appellation/DuelContWinAppellation_Common"

local StmtContainer 		= class()
local DuelContWinAppellation_Common = DuelContWinAppellation_Common

local ChallengeFlagDB = CreateDbBox(...)
---------------------------------------------------------------------
local StmtDef = 
{
	"_QueryDuelWinCount",
	"select cs_uDuelWinCount from tbl_char_status where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

--@brief 查询胜利后是否获得称谓
function ChallengeFlagDB.QueryDuelWinCount(data)
	local result = StmtContainer._QueryDuelWinCount:ExecStat(data["char_id"])
	local charLevel = data["char_level"]
	
	local DuelWinCount = result:GetData(0,0)
	result:Release()
	local Content = nil
	local minLevelKeys = DuelContWinAppellation_Common:GetKeys()
	for _, minLevel in pairs(minLevelKeys) do
		local firstData = DuelContWinAppellation_Common(minLevel)
		local maxLevelKeys = firstData:GetKeys()
		for _, maxLevel in pairs(maxLevelKeys) do
			if charLevel >= tonumber(minLevel) and charLevel <= tonumber(maxLevel) then
				local secData = firstData(maxLevel)
				local winCountKeys = secData:GetKeys()
				for _, winCount in pairs(winCountKeys) do
					local maxWinCount = secData(winCount,"MaxWinCount")
					if DuelWinCount >= tonumber(winCount) and (DuelWinCount <= maxWinCount or maxWinCount == -1) then
						Content = secData(winCount,"Content")
						return DuelWinCount,Content
					end
				end
			end
		end
	end
	return DuelWinCount,Content
end
----------------------------------------------------------------------
local StmtDef = 
{
	"_UpdateDuelWinCount",
	"update tbl_char_status set cs_uDuelWinCount = ? where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef = 
{
	"_QueryDuelRecord",
	"select count(*)  from tbl_duel_count where cs_uId = ? and dc_uChallengerId = ? and date(now()) = date(dc_dtTime)"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef = 
{
	"InsertDuelRecord",
	"replace into tbl_duel_count(cs_uId, dc_uChallengerId, dc_dtTime)values(?,?,now())"
}
DefineSql(StmtDef,StmtContainer)

--@brief 挑战成功后更新胜利次数相关信息
function ChallengeFlagDB.UpdateDuelWinCount(data)
	local DuelWinCount	= data["DuelWinCount"]
	local charID 				= data["char_id"]
	local challengerId	= data["challengerId"]
	local charLevel			= data["char_level"]
	
	local FightingEvaluationDB		= RequireDbBox("FightingEvaluationDB")
	local LogMgrDB = RequireDbBox("LogMgrDB")

	local param1 = {["charId"] = charID,["uTotalPoint"] = data["playerFightingTotalPoint"]}
	FightingEvaluationDB.SaveFightingEvaluation(param1)
	local param2 = {["charId"] = challengerId,["uTotalPoint"] = data["challengerFightingTotalPoint"]}
	FightingEvaluationDB.SaveFightingEvaluation(param2)
	
	local playerInfo = LogMgrDB.GetCharSaveLogInfo(charID)
	local challengerInfo = LogMgrDB.GetCharSaveLogInfo(challengerId)
	
	local playerInfoTbl = {charID,playerInfo[1],playerInfo[2],playerInfo[3],data["playerInfo"][1],data["playerInfo"][2],data["playerInfo"][3],data["playerInfo"][4]}
	local challengerInfoTbl = {challengerId,challengerInfo[1],challengerInfo[2],challengerInfo[3],data["challengerInfo"][1],data["challengerInfo"][2],data["challengerInfo"][3],data["challengerInfo"][4]}
	
	if DuelWinCount == 0 then
		StmtContainer._UpdateDuelWinCount:ExecStat(0, charID)
		LogMgrDB.SaveCharPKInfoLog(data["uFlagId"], charID, challengerId,data["time"],0,playerInfoTbl,challengerInfoTbl)
		return DuelWinCount
	else
		local result = StmtContainer._QueryDuelRecord:ExecStat(charID,challengerId)
		local count =  result:GetData(0,0)
		local winCountRet = StmtContainer._QueryDuelWinCount:ExecStat(charID)
		local DuelWinCount = winCountRet:GetData(0,0)
		if count > 0 then
			LogMgrDB.SaveCharPKInfoLog(data["uFlagId"], charID, challengerId,data["time"],1,playerInfoTbl,challengerInfoTbl)
			return DuelWinCount
		else
			DuelWinCount = DuelWinCount + 1
			StmtContainer.InsertDuelRecord:ExecStat(charID, challengerId)
			StmtContainer._UpdateDuelWinCount:ExecStat(DuelWinCount, charID)
			
			local Content = nil
			local minLevelKeys = DuelContWinAppellation_Common:GetKeys()
			for _, minLevel in pairs(minLevelKeys) do
				local firstData = DuelContWinAppellation_Common(minLevel)
				local maxLevelKeys = firstData:GetKeys()
				for _, maxLevel in pairs(maxLevelKeys) do
					if charLevel >= tonumber(minLevel) and charLevel <= tonumber(maxLevel) then
						local secData = firstData(maxLevel)
						local winCountKeys = secData:GetKeys()
						for _, winCount in pairs(winCountKeys) do
							local maxWinCount = secData(winCount,"MaxWinCount")
							if DuelWinCount >= tonumber(winCount) and (DuelWinCount <= maxWinCount or maxWinCount == -1) then
								Content = secData(winCount,"Content")
								if Content ~= nil then
									data["AppellationText"] = Content
									local GasAppellationAndMoralDB = RequireDbBox("GasAppellationAndMoralDB")
									if not GasAppellationAndMoralDB.QueryPlayerAppellation(data) then
										LogMgrDB.SaveCharPKInfoLog(data["uFlagId"], charID, challengerId,data["time"],1,playerInfoTbl,challengerInfoTbl)
										return DuelWinCount,Content,GasAppellationAndMoralDB.AddNewAppellation(data)
									end
								end
							end
						end
					end
				end
			end
			
			LogMgrDB.SaveCharPKInfoLog(data["uFlagId"], charID, challengerId,data["time"],1,playerInfoTbl,challengerInfoTbl)
			return DuelWinCount,Content
		end
	end
end

SetDbLocalFuncType(ChallengeFlagDB.QueryDuelWinCount)
SetDbLocalFuncType(ChallengeFlagDB.UpdateDuelWinCount)

return ChallengeFlagDB
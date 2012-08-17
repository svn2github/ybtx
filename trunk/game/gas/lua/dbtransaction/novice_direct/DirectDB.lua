cfg_load "player/Direct_Common"
gac_gas_require "activity/direct/LoadDirectCommon"

local Direct_Common = Direct_Common
local DirectKeys = Direct_Common:GetKeys()
local g_DirectMgr = g_DirectMgr
local g_ItemInfoMgr = g_ItemInfoMgr
local g_AllMercenaryQuestMgr = g_AllMercenaryQuestMgr
local g_TeamMercenaryQuestMgr = g_TeamMercenaryQuestMgr
local g_HideQuestMgr = g_HideQuestMgr
local Npc_Common = Npc_Common
local StmtContainer = class()

local StmtDef=
{
	"_GetPlayerAllDirect",
	"select cd_sDirectName, cd_uState from tbl_char_direct where cs_uId = ?"
}
DefineSql(StmtDef, StmtContainer)

local StmtDef=
{
	"_PlayerDirectIsExist",
	"select cd_uState from tbl_char_direct where cs_uId = ? and cd_sDirectName = ?"
}
DefineSql(StmtDef, StmtContainer)

local StmtDef=
{
	"_InsertPlayerDirect",
	"insert into tbl_char_direct (cs_uId, cd_sDirectName) values (?,?)"
}
DefineSql(StmtDef, StmtContainer)

local StmtDef=
{
	"_GetPlayerAllDirectState",
	"select cds_sDirectName, cds_uCount from tbl_char_direct_state where cs_uId = ?"
}
DefineSql(StmtDef, StmtContainer)

local StmtDef=
{
	"_PlayerDirectStateIsExist",
	"select cds_uCount from tbl_char_direct_state where cs_uId = ? and cds_sDirectName = ?"
}
DefineSql(StmtDef, StmtContainer)

local StmtDef=
{
	"_InsertPlayerDirectState",
	"insert into tbl_char_direct_state (cs_uId, cds_sDirectName, cds_uCount) values (?,?,1)"
}
DefineSql(StmtDef, StmtContainer)

local StmtDef=
{
	"_UpdatePlayerDirectState",
	"update tbl_char_direct_state set cds_uCount = cds_uCount + 1 where cs_uId = ? and cds_sDirectName = ?"
}
DefineSql(StmtDef, StmtContainer)

local StmtDef=
{
	"_DeletePlayerDirectState",
	"delete from tbl_char_direct_state where cs_uId = ? and cds_sDirectName = ?"
}
DefineSql(StmtDef, StmtContainer)
--============================================================================--

local StmtDef=
{
	"_FinishPlayerDirect",
	"update tbl_char_direct set cd_uState = -1 where cs_uId = ? and cd_sDirectName = ?"
}
DefineSql(StmtDef, StmtContainer)
--============================================================================--

local StmtDef=
{
	"_GetPlayerAllDirectAward",
	"select cda_sAwardName, cda_uState from tbl_char_direct_award where cs_uId = ?"
}
DefineSql(StmtDef, StmtContainer)

local StmtDef=
{
	"_PlayerDirectAwardIsExist",
	"select * from tbl_char_direct_award where cs_uId = ? and cda_sAwardName = ?"
}
DefineSql(StmtDef, StmtContainer)

local StmtDef=
{
	"_InsertPlayerDirectAward",
	"insert into tbl_char_direct_award (cs_uId, cda_sAwardName, cda_uState) values (?,?,0)"
}
DefineSql(StmtDef, StmtContainer)

local StmtDef=
{
	"_GetPlayerDirectAward",
	"update tbl_char_direct_AwardItem set cda_uState = 1 where cs_uId = ? and cda_sAwardName = ?"
}
DefineSql(StmtDef, StmtContainer)


local DirectDB = CreateDbBox(...)

function DirectDB.FinishPlayerDirect(data)
	local CharID = data["CharID"]
	local DirectName = data["DirectName"]
	
	local result = StmtContainer._PlayerDirectIsExist:ExecStat(CharID,DirectName)
	if result:GetRowNum() == 0 then
		return 1, DirectDB.GetPlayerAllDirectInfo(CharID)
	end
	if result:GetData(0,0) ~= 0 then
		return 2, DirectDB.GetPlayerAllDirectInfo(CharID)
	end
	StmtContainer._FinishPlayerDirect:ExecStat(CharID, DirectName)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

function DirectDB.GetDirectAwardItem(data)

	local CharID = data["CharID"]
	local BigType = data["BigType"]
	local ActionType = data["ActionType"]
	local DirectInfo = g_DirectMgr.m_DirectInfoTbl
	local nActionType = g_DirectMgr.m_tblActionTypeTrans[ActionType]
	local DirectList = DirectInfo[BigType][nActionType]["DirectList"]
	StmtContainer._PlayerDirectAwardIsExist:ExecStat(CharID, ActionType)
	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
		return 3
	end
	for i , v in pairs(DirectList) do
		local result = StmtContainer._PlayerDirectIsExist:ExecStat(CharID, v)
		if result:GetRowNum() == 0 then
			return 1, DirectDB.GetPlayerAllDirectInfo(CharID)
		end
		if result:GetData(0,0) ~= -1 then
			return 2, DirectDB.GetPlayerAllDirectInfo(CharID)
		end
	end
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local param = {
		["PlayerId"] = CharID,
		["AddItemsTbl"] = DirectInfo[BigType][nActionType]["AwardItem"],
		["sceneName"] = data["sceneName"],
		["createType"] = data["createType"]
	}
	
	local res, returnTbl = CharacterMediatorDB.GetItems(param)
	if res then
		StmtContainer._InsertPlayerDirectAward:ExecStat(CharID, ActionType)
	end
	return res, returnTbl
end

function DirectDB.GetPlayerAllDirectInfo(CharID)
	local result = {}
	
	local res1 = StmtContainer._GetPlayerAllDirect:ExecStat(CharID)
	if res1:GetRowNum() > 0 then
		local DirectTbl = res1:GetTableSet()
		for i= 1,res1:GetRowNum() do
			result[DirectTbl(i,1)] = DirectTbl(i,2)
		end
	end
	
	local res2 = StmtContainer._GetPlayerAllDirectState:ExecStat(CharID)
	if res2:GetRowNum() > 0 then
		local DirectTbl = res2:GetTableSet()
		for i= 1,res2:GetRowNum() do
			if not result[DirectTbl(i,1)] then
				result[DirectTbl(i,1)] = DirectTbl(i,2)
			end
		end
	end
	
	return result
end

function DirectDB.GetPlayerAllDirectAward(CharID)
	local result = {}
	
	local res = StmtContainer._GetPlayerAllDirectAward:ExecStat(CharID)
	if res:GetRowNum() > 0 then
		local AwardTbl = res:GetTableSet()
		for i= 1,res:GetRowNum() do
			result[AwardTbl(i,1)] = AwardTbl(i,2)
		end
	end
	
	return result
end

function DirectDB.UpdatePlayerDirect(data)
	return DirectDB.GetPlayerAllDirectInfo(data["CharID"])
end

function DirectDB.InsertPlayerDirect(CharID, DirectName)
	StmtContainer._InsertPlayerDirect:ExecStat(CharID,DirectName)
	--print("_InsertPlayerDirect",CharID,DirectName)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

function DirectDB.AddPlayerDirectState(CharID, DirectName)
	local result = StmtContainer._PlayerDirectStateIsExist:ExecStat(CharID, DirectName)
	if result:GetRowNum() == 0 then
		StmtContainer._InsertPlayerDirectState:ExecStat(CharID, DirectName)
		--print("_InsertPlayerDirectState",CharID, DirectName)
	else
		StmtContainer._UpdatePlayerDirectState:ExecStat(CharID, DirectName)
		--print("_UpdatePlayerDirectState",CharID, DirectName)
	end
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

function DirectDB.AddPlayerDirect(CharID, DirectName)

	local result = StmtContainer._PlayerDirectIsExist:ExecStat(CharID,DirectName)
	if result:GetRowNum() == 0 then
		local Count = Direct_Common(DirectName, "Count")
		if Count == 0 then
			DirectDB.InsertPlayerDirect(CharID, DirectName)
			return DirectName
		end
		local StateRes = DirectDB.AddPlayerDirectState(CharID, DirectName)
		if StateRes then
			result = StmtContainer._PlayerDirectStateIsExist:ExecStat(CharID, DirectName)
			local oldCount = result:GetData(0,0)
			if oldCount >= Count then
				DirectDB.InsertPlayerDirect(CharID, DirectName)
				StmtContainer._DeletePlayerDirectState:ExecStat(CharID, DirectName)
				return DirectName
			end
			return DirectName, oldCount
		end
	else
		--print("_PlayerDirectIsExist",CharID,DirectName)
	end
end

function DirectDB.AddPlayerQuestDirect(CharID, Action, QuestName, DirectTbl)
--屏蔽1
	local function IsQuestType(QuestName, Type, Arg)
		local QuestType = {
			["任务类型"] = function(QuestName, Arg)
				if Arg == "佣兵任务" then
					return g_AllMercenaryQuestMgr[QuestName]
				elseif Arg == "佣兵小队任务" then
					return g_TeamMercenaryQuestMgr[QuestName]
				elseif Arg == "隐藏任务" then
					return g_HideQuestMgr[QuestName]
				end
			end,
			["任务名"] = function(QuestName, Arg) if Arg == QuestName then return true end end,
			["部分任务名"] = function(QuestName, Arg) if (string.find(QuestName, Arg)) then return true end end,
		}
		if QuestType[Type] and QuestType[Type](QuestName, Arg) then
			return true
		end
	end

	local ResultTbl = DirectTbl or {}
--	local Keys = Direct_Common:GetKeys()
	for _, i in pairs(DirectKeys) do
		if Direct_Common(i, "Action") == Action then
			if Direct_Common(i, "Type") == "" or IsQuestType(QuestName, Direct_Common(i, "Type"), Direct_Common(i, "Arg")) then
				local DirectName, Count = DirectDB.AddPlayerDirect(CharID, i)
				if DirectName then
					 ResultTbl[DirectName] = Count or 0
				end
			end
		end
	end
	
	return ResultTbl
--屏蔽2
end

function DirectDB.AddPlayerKillNpcDirect(CharID, Action, NpcName, DirectTbl)
--屏蔽1
	local function IsNpcType(NpcName, Type, Arg)
		local NpcType = {
			["Npc名称"] = function(Name) return NpcName == Arg end ,
			["Npc职业"] = function(Name) return Npc_Common(Name,"Class") == tonumber(Arg) end
		}
		if NpcType[Type] then
			return NpcType[Type](NpcName)
		end
	end
	
	local ResultTbl = DirectTbl or {}
--	local Keys = Direct_Common:GetKeys()
	for _, i in pairs(DirectKeys) do
		if Direct_Common(i, "Action") == Action then
			if Direct_Common(i, "Type") == "" or IsNpcType(NpcName, Direct_Common(i, "Type"), Direct_Common(i, "Arg")) then
				local DirectName, Count = DirectDB.AddPlayerDirect(CharID, i)
				if DirectName then
					 ResultTbl[DirectName] = Count or 0
				end
			end
		end
	end
	
	return ResultTbl
--屏蔽2
end

function DirectDB.AddPlayerJifenDirect(CharID, Action, JifenType, Jifen, DirectTbl)
--屏蔽1
	local function IsJifenType(JifenType, Jifen, Type, Arg)
		local JifenTypeTbl = {
			["积分类型"] = function(JifenType, Jifen, Arg) if JifenType == tonumber(Arg) then return true end end ,
			["积分数值"] = function(JifenType, Jifen, Arg) if Jifen >= tonumber(Arg) then return true end end
		}
		if JifenTypeTbl[Type] then
			return JifenTypeTbl[Type](JifenType, Jifen, Arg)
		end
	end
	
	local ResultTbl = DirectTbl or {}
--	local Keys = Direct_Common:GetKeys()
	for _, i in pairs(DirectKeys) do
		if Direct_Common(i, "Action") == Action then
			if Direct_Common(i, "Type") == "" or IsJifenType(JifenType, Jifen, Direct_Common(i, "Type"), Direct_Common(i, "Arg")) then
				local DirectName, Count = DirectDB.AddPlayerDirect(CharID, i)
				if DirectName then
					 ResultTbl[DirectName] = Count or 0
				end
			end
		end
	end
	
	return ResultTbl
--屏蔽2
end

function DirectDB.AddPlayerItemDirect(CharID, Action, ItemType, ItemName, DirectTbl)
--屏蔽1
	local function IsItemType(ItemType, ItemName, Type, Arg)
		local ItemTypeTbl = {
			["物品大类"] = function(ItemType, ItemName, Quality, Arg) if ItemType == tonumber(Arg) then return true end end ,
			["物品名称"] = function(ItemType, ItemName, Quality, Arg) if ItemName == Arg then return true end end ,
			["物品品质"] = function(ItemType, ItemName, Quality, Arg) if Quality == tonumber(Arg) then return true end end
		}
		if ItemTypeTbl[Type] then
			local Quality = g_ItemInfoMgr:GetItemInfo(ItemType, ItemName,"Quality")
			return ItemTypeTbl[Type](ItemType, ItemName,Quality, Arg)
		end
	end
	
	local ResultTbl = DirectTbl or {}
--	local Keys = Direct_Common:GetKeys()
	for _, i in pairs(DirectKeys) do
		if Direct_Common(i, "Action") == Action then
			if Direct_Common(i, "Type") == "" or IsItemType(ItemType, ItemName, Direct_Common(i, "Type"), Direct_Common(i, "Arg")) then
				local DirectName, Count = DirectDB.AddPlayerDirect(CharID, i)
				if DirectName then
					 ResultTbl[DirectName] = Count or 0
				end
			end
		end
	end
	
	return ResultTbl
--屏蔽2
end

function DirectDB.AddPlayerLevelDirect(CharID, Action, Level, DirectTbl)
--屏蔽1
	local function IsLevelType(Level, Type, Arg)
		local LevelTbl = {
			["达到等级"] = function(Level, Arg) if Level >= tonumber(Arg) then return true end end ,
		}
		if LevelTbl[Type] then
			return LevelTbl[Type](Level, Arg)
		end
	end
	
	local ResultTbl = DirectTbl or {}
--	local Keys = Direct_Common:GetKeys()
	for _, i in pairs(DirectKeys) do
		if Direct_Common(i, "Action") == Action then
			if Direct_Common(i, "Type") == "" or IsLevelType(Level, Direct_Common(i, "Type"), Direct_Common(i, "Arg")) then
				local DirectName, Count = DirectDB.AddPlayerDirect(CharID, i)
				if DirectName then
					ResultTbl[DirectName] = Count or 0
				end
			end
		end
	end
	Db2CallBack("PlayerFinishDirect",CharID, ResultTbl)
--屏蔽2
end

function DirectDB.AddPlayerDirectAction(data)
--屏蔽1
	local CharID = data["CharID"]
	local Action = data["Action"]
	local Type = data["Type"]
	local Arg = data["Arg"]
	
	local ResultTbl = DirectTbl or {}
--	local Keys = Direct_Common:GetKeys()
	for _, i in pairs(DirectKeys) do
		if Direct_Common(i, "Action") == Action then
			if Direct_Common(i, "Type") == Type then
				if Direct_Common(i, "Arg") == Arg then
					local DirectName, Count = DirectDB.AddPlayerDirect(CharID, i)
					if DirectName then
						ResultTbl[DirectName] = Count or 0
					end
				end
			end
		end
	end
	
	return ResultTbl
--屏蔽2
end

SetDbLocalFuncType(DirectDB.FinishPlayerDirect)
SetDbLocalFuncType(DirectDB.GetDirectAwardItem)
SetDbLocalFuncType(DirectDB.AddPlayerDirectAction)
SetDbLocalFuncType(DirectDB.UpdatePlayerDirect)

return DirectDB

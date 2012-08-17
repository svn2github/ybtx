gac_gas_require "activity/quest/QuestMgrInc"
gas_require "world/player/CreateServerPlayerInc"
gac_gas_require "item/store_room_cfg/StoreRoomCfg"
gas_require "message/ServerMsg"
cfg_load "item/Mercenary_Quest_Server"
--cfg_load "player/MercenaryLevel_Common"
cfg_load "player/LevelExp_Common"
cfg_load "tong/TongLevelModulus_Server"
cfg_load "tong/TongStationLevelModulus_Server"
cfg_load "skill/SkillPart_Common"
cfg_load "item/TongProdItem_Common"
gac_gas_require "scene_mgr/SceneCfg"
gac_gas_require "relation/tong/TongMgr"
gac_gas_require "framework/common/CMoney"
gac_gas_require "activity/quest/MercenaryLevelCfg"
gac_gas_require "skill/SkillCommon"

local Scene_Common = Scene_Common
local SkillPart_Common = SkillPart_Common
local Quest_Common = Quest_Common
local g_MercenaryLevelTbl = g_MercenaryLevelTbl
local g_QuestNeedMgr = g_QuestNeedMgr
local g_QuestPropMgr = g_QuestPropMgr
local g_AreaQuestMgr = g_AreaQuestMgr
local g_RepeatQuestMgr = g_RepeatQuestMgr
local g_MasterStrokeQuestMgr = g_MasterStrokeQuestMgr
local g_LoopQuestMgr = g_LoopQuestMgr
local LevelExp_Common = LevelExp_Common
--local MercenaryLevel_Common = MercenaryLevel_Common
local Mercenary_Quest_Server = Mercenary_Quest_Server
local g_ItemInfoMgr = CItemInfoMgr:new()
local g_WhereGiveQuestMgr = g_WhereGiveQuestMgr
local g_MoneyMgr = CMoney:new()
local TongLevelModulus_Server = TongLevelModulus_Server
local TongStationLevelModulus_Server = TongStationLevelModulus_Server
local event_type_tbl = event_type_tbl
local QuestState = {
	init	= 1,
	failure	= 2,
	finish	= 3
}

local stateTbl = {
	["安装三合一豪华版状态"] = {3, "all"},
	["三倍经验的buff"] = {3, "exp"},
	["安装五合一卡状态"] = {5, "all"},
	["双倍道具符"] = {2, "item"},
}

local g_ItemQuestMgr = g_ItemQuestMgr
--local g_QuestAwardMgr = g_QuestAwardMgr
local g_VarQuestMgr = g_VarQuestMgr
local os = os
local g_StoreRoomIndex = g_StoreRoomIndex
local CPos = CPos
local g_KillNpcQuestMgr = g_KillNpcQuestMgr
local loadstring = loadstring
local ECamp = ECamp
local LogErr = LogErr
local g_AllMercenaryQuestMgr = g_AllMercenaryQuestMgr
local g_DareMercenaryQuestMgr = g_DareMercenaryQuestMgr
local g_DareQuestMgr = g_DareQuestMgr
local g_ActionQuestMgr = g_ActionQuestMgr
--local GetHDProcessTime = GetHDProcessTime
--Renton:引进技能类型变量
local g_TongMgr = g_TongMgr or CTongMgr:new()
local FightSkillKind = FightSkillKind
--local FightSkill = CreateDbBox(...)
local GetCfgTransformValue = GetCfgTransformValue
local GetStaticTextServer = GetStaticTextServer
local TongProdItem_Common = TongProdItem_Common


local g_TestRevisionMaxLevel = g_TestRevisionMaxLevel --测试版本最大等级

local StmtContainer = class()

local ShareArea = 38    -- 共享经验和任务计数范围  2009-7-16

local StmtDef = 
{
	"_AddNewQuest",
	[[
		insert ignore into tbl_quest(q_sName,cs_uId,q_state,q_tAcceptTime,q_uExpMultiple) values ( ?, ?, 1, ?, ? )
	]]
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = {
	"_AddNewQuestVar",
	[[
		insert ignore into tbl_quest_var (q_sName,cs_uId,qv_name) values(?,?,?)
	]]
}
DefineSql( StmtDef, StmtContainer)

local StmtDef = 
{
	"_ClearQuest",
	[[
		delete from tbl_quest where q_sName = ? and cs_uId = ?
	]]
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = {
	"_DeleteAllQuestVar",
	[[
		delete from tbl_quest_var where q_sName = ? and cs_uId = ? 
	]]
}
DefineSql( StmtDef, StmtContainer)

local StmtDef = {
	"_GetQuestAcceptTime",
	[[
		select q_tAcceptTime from tbl_quest where q_sName=? and cs_uId=?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_GetQuestState",
	[[
		select
			q_state,q_uLimitTime
		from
			tbl_quest
		where
			cs_uId = ? and q_sName = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_GetQuestStateFinishTime",
	[[
		select
			q_state,q_tFinishTime
		from
			tbl_quest
		where
			cs_uId = ? and q_sName = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_GetDoingQuests",
	[[
		select q_sName 
		from tbl_quest
		where cs_uId = ? and q_state = 1
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_GetDoingQuestNum",
	[[
		select q_sName 
		from tbl_quest
		where cs_uId = ? and (q_state=1 or q_state=2)
	]]
}
DefineSql( StmtDef, StmtContainer )
 
local StmtDef = {
	"_GetQuestExpMultiple",
	[[
		select q_uExpMultiple from tbl_quest where cs_uId = ? and q_sName = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_UpdateQuestState",
	[[
		update tbl_quest set q_state = ?,q_tFinishTime = ? where cs_uId = ? and q_sName = ?  
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_AddCharFinishQuestNum",
	[[
		update tbl_char_quest_finish_num set cqfn_uFinishNum = cqfn_uFinishNum+? where cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_InsertCharFinishQuestNum",
	[[
		insert into tbl_char_quest_finish_num (cs_uId,cqfn_uFinishNum) values(?,?)
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_SelectCharFinishQuestNum",
	[[
		select cqfn_uFinishNum from tbl_char_quest_finish_num where cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_UpdateQuestTime",
	[[
		update tbl_quest set q_tAcceptTime = ?, q_uLimitTime = 0 where cs_uId = ? and q_sName = ?  
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_SelectQuestVarNum",
	[[
		select qv_num from tbl_quest_var where q_sName = ? and qv_name = ? and cs_uId = ? 
	]]
}
DefineSql( StmtDef, StmtContainer)

local StmtDef = {
	"_SelectQNameVNameVNum",
	[[
		select qv_name,qv_num from tbl_quest_var where  q_sName=? and cs_uId = ? 
	]]
}
DefineSql( StmtDef, StmtContainer)

--update tbl_quest_var set qv_num = qv_num + ? where q_sName = ? and qv_name = ? and cs_uId = ? and qv_num + ? <= ?
local StmtDef = {
	"_AddQuestVarNum",
	[[
		update 
			tbl_quest_var as qv,tbl_quest as qs 
		set 
			qv.qv_num = qv.qv_num + ?
		where
			qv.q_sName = qs.q_sName and qv.cs_uId = qs.cs_uId and qs.q_state = 1 and qv.cs_uId = ? and qv.q_sName = ?  and qv.qv_name = ? and qv.qv_num + ? <= ?
	]]
}
DefineSql( StmtDef, StmtContainer)

local StmtDef = {
	"_SetQuestVarNum",
	[[
		update 
			tbl_quest_var as qv,tbl_quest as qs 
		set 
			qv.qv_num = ?
		where
			qv.q_sName = qs.q_sName and qv.cs_uId = qs.cs_uId and qs.q_state = 1 and qv.cs_uId = ? and qv.q_sName = ?  and qv.qv_name = ?
	]]
}
DefineSql( StmtDef, StmtContainer)

local StmtDef = {
	"_DeleteQuestVar",
	[[
		delete from tbl_quest_var where q_sName = ? and cs_uId = ? and qv_name = ?
	]]
}
DefineSql( StmtDef, StmtContainer)

local StmtDef = {
	"_GiveUpQuestSql",
	[[
		delete from tbl_quest where q_sName = ? and cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef =
{
	"_GetPlayerLevel",
	"select cb_uLevel from tbl_char_basic where cs_uId = ? "
}
DefineSql( StmtDef , StmtContainer )
------------------------------------------

local StmtDef = {
	"_GetAllQuestVar",
	[[
		select q_sName, qv_name, qv_num 
		from tbl_quest_var
		where cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_GetAllQuestSql",
	[[
		select q_sName, q_tAcceptTime, q_state, q_uLimitTime from tbl_quest where cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_GetNowQuestSql",
	[[
		select 
			s.q_sName, v.qv_name, v.qv_num
		from 
			tbl_quest as s, tbl_quest_var as v
		where 
			s.cs_uId = v.cs_uId and s.q_sName = v.q_sName
			and s.cs_uId = ? and s.q_state= 1
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = 
{
	"_GetPlayerDoingQstNameByState",
	[[
		select q_sName from tbl_quest where q_state = ? and cs_uId = ?
	]]
	
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = 
{
	"GetQuestLimit",
	[[
		select
			ql_Count, unix_timestamp(ql_CountTime), unix_timestamp(ql_CDTime)
		from tbl_quest_limit
		where q_sName = ? and cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = 
{
	"UpdateQuestLimit",
	[[
		update
			tbl_quest_limit
		set
			ql_Count = ?,
			ql_CountTime = from_unixtime(?),
			ql_CDTime = from_unixtime(?)
		where
			q_sName = ? and cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = 
{
	"InsertQuestLimit",
	[[
		insert into tbl_quest_limit
			(q_sName, cs_uId, ql_Count, ql_CountTime, ql_CDTime)
		values
			(?, ?, ?, from_unixtime(?), from_unixtime(?))
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"DeleteQuestLimit",
	[[
		delete from tbl_quest_limit where q_sName = ? and cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer)

local StmtDef=
{
	"_CountQuestLimitByName",
	[[
		select count(*) from tbl_quest_finish_limit where qfl_sQuestName = ?
	]]
}
DefineSql( StmtDef, StmtContainer)

local StmtDef=
{
	"_CountObjDropLimitByName",
	[[
		select count(*) from tbl_obj_drop_limit where odl_sObjName = ?
	]]
}
DefineSql( StmtDef, StmtContainer)

local StmtDef=
{
	"_GetAllQuestLoop",
	[[
		select ql_sLoopName, ql_uLoopNum from tbl_quest_loop where cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer)

local StmtDef=
{
	"_GetQuestLoop",
	[[
		select ql_uLoopNum from tbl_quest_loop where cs_uId = ? and ql_sLoopName = ?
	]]
}
DefineSql( StmtDef, StmtContainer)

local StmtDef=
{
	"_UpdateQuestLoop",
	[[
		replace into tbl_quest_loop (cs_uId, ql_sLoopName, ql_uLoopNum) values (?,?,?)
	]]
}
DefineSql( StmtDef, StmtContainer)

local StmtDef=
{
	"_DeleteQuestLoop",
	[[
		delete from tbl_quest_loop where cs_uId = ? and ql_sLoopName = ?
	]]
}
DefineSql( StmtDef, StmtContainer)

----------------------------------------------

local RoleQuestDB = CreateDbBox(...)

-----------------------------------------------------------------------------------
local function SetResult(OneRet,nRoom,nPos,nCount,nBigID,nIndex)
	assert(IsTable(OneRet))
	OneRet.m_nRoomIndex = nRoom
	OneRet.m_nPos = nPos
	OneRet.m_nCount = nCount
	OneRet.m_nBigID = nBigID
	OneRet.m_nIndex = nIndex
	return OneRet
end

local function Get2PosDistance(pos1, pos2)
	return math.max( math.abs( pos1.x - pos2.x ), math.abs( pos1.y - pos2.y ) )
end

local function IfAcceptQuestMinLev(PlayerLevel, QuestName)
	local minlevel = Quest_Common(QuestName, "接任务最小等级")
	if (minlevel and minlevel ~= 0 and PlayerLevel < minlevel) then
		return
	end
	return true
end

local function IfAcceptQuestMaxLev(PlayerLevel, QuestName)
	local maxlevel = Quest_Common(QuestName, "接任务最高等级")
	if (maxlevel and maxlevel ~= 0 and PlayerLevel > maxlevel) then
		return false
	end
	return true
end

local function CmpMercenaryLevel(PlayerLevel, QuestName)
	--local Level = Quest_Common(QuestName, "接任务最小佣兵等级")
	--if Level and Level~="" then
		--local iLevel = Quest
		--for i=1, #(MercenaryLevel_Common) do
		--	if MercenaryLevel_Common[i].MercenaryLevel == iLevel then
		--		iLevel = i
		--		break
		--	end
		--end
		--if (iLevel - PlayerLevel) > 5 then
		--	return false
		--end
	--end
	return true
end

local function IfAcceptQuestCamp(PlayerCamp, QuestName)
	local CampLimit = Quest_Common(QuestName, "阵营限制")
	if CampLimit and CampLimit ~= 0 and CampLimit ~= PlayerCamp then
		return false
	end
	return true
end

local function CheckQuestTimeCount(QuestName, PlayerId, TimeTbl, FinishCount)
	local nowTime = os.time()
	local sDate = TimeTbl[1]
	local sTime = TimeTbl[2]
	local oDate = TimeTbl[4]
	local oTime = TimeTbl[5]
	local startTime, overTime
	
	local j = string.find(sDate, "/")
	local k = string.find(sDate, "/", j+1)
	local tbl = {}
	tbl.month = tonumber( string.sub(sDate, 1, j-1) )
	tbl.day   = tonumber( string.sub(sDate, j+1, k-1) )
	tbl.year  = tonumber( string.sub(sDate, k+1) )
	j = string.find(sTime, ":")
	tbl.hour  = tonumber( string.sub(sTime, 1, j-1) )
	tbl.min   = tonumber( string.sub(sTime, j+1) )
	startTime = os.time(tbl)
	
	if oDate and oTime then
		j = string.find(oDate, "/")
		k = string.find(oDate, "/", j+1)
		tbl.month = tonumber( string.sub(oDate, 1, j-1) )
		tbl.day   = tonumber( string.sub(oDate, j+1, k-1) )
		tbl.year  = tonumber( string.sub(oDate, k+1) )
		j = string.find(oTime, ":")
		tbl.hour  = tonumber( string.sub(oTime, 1, j-1) )
		tbl.min   = tonumber( string.sub(oTime, j+1) )
		overTime = os.time(tbl)
	end
	
	if nowTime < startTime then
		return false, true	--没到任务接取时间
	end
	if overTime and nowTime > overTime then
		return nil, true	--任务活动已结束
	end
	
	local res = StmtContainer.GetQuestLimit:ExecStat(QuestName, PlayerId)
	if res:GetRowNum() == 0 then
		StmtContainer.InsertQuestLimit:ExecStat(QuestName, PlayerId, 0, nowTime, 0)
		res:Release()
		return true, true		--数据库没有，可以接取任务
	else
		local count     = res:GetData(0,0)
		local countTime = res:GetData(0,1)
		local cdTime    = res:GetData(0,2)
		local cycle = TimeTbl[3]*3600
		local sign1, sign2 = false, false
		if math.floor((nowTime-startTime)/cycle) ~= math.floor((countTime-startTime)/cycle) then
			count = 0
			StmtContainer.UpdateQuestLimit:ExecStat(count, nowTime, cdTime, QuestName, PlayerId)
			sign2 = true
		end
		if count < FinishCount then
			sign1 = true
		end
		res:Release()
		return sign1, sign2
	end
end

local function AddQuestTimeCount(questName, playerId)
	local res = StmtContainer.GetQuestLimit:ExecStat(questName, playerId)
	if res:GetRowNum() ~= 0 then
		local	count     = res:GetData(0,0)
		local countTime = res:GetData(0,1)
		local	cdTime    = res:GetData(0,2)
		StmtContainer.UpdateQuestLimit:ExecStat(count+1, countTime, cdTime, questName, playerId)
	end
	res:Release()
end

local function CheckQuestCD(questName, playerId, CDTime)
	local nowTime = os.time()
	local res = StmtContainer.GetQuestLimit:ExecStat(questName, playerId)
	if res:GetRowNum() == 0 then
		StmtContainer.InsertQuestLimit:ExecStat(questName, playerId, 0, 0, nowTime)
		res:Release()
		return true		--数据库没有，可以接取任务
	else
		local count     = res:GetData(0,0)
		local countTime = res:GetData(0,1)
		local cdTime    = res:GetData(0,2)
		if nowTime-cdTime >= CDTime*3600 then
			StmtContainer.UpdateQuestLimit:ExecStat(count, countTime, nowTime, questName, playerId)
			res:Release()
			return true
		else
			local date = os.date("*t", CDTime*3600-(nowTime-cdTime))
			local defDate = os.date("*t", 0)
			local str = (date.hour-defDate.hour)..GetStaticTextServer(1121)..(date.min-defDate.min)..GetStaticTextServer(1120)
			res:Release()
			return false, str
		end
	end
end

local function RefreshQuestCD(questName, playerId)
	local res = StmtContainer.GetQuestLimit:ExecStat(questName, playerId)
	if res:GetRowNum() ~= 0 then
		local	count     = res:GetData(0,0)
		local countTime = res:GetData(0,1)
		StmtContainer.UpdateQuestLimit:ExecStat(count, countTime, 0, questName, playerId)
	end
	res:Release()
end

local function CheckStartTime(sQuestName)
	local tbl = GetCfgTransformValue(true, "Quest_Common", sQuestName, "任务接取时间", "Subject")
	local WDayTbl = tbl.wday
	local StartTimeTbl = tbl.time
	local LastTime = Quest_Common(sQuestName, "任务接取时间", "Arg")
	local Date = os.date("*t")
	WDayTbl = WDayTbl or {1,2,3,4,5,6,7}
	for i=1,#WDayTbl do
		local wday = (WDayTbl[i]==7 and 1) or WDayTbl[i]+1
		if wday == Date.wday then
			for j=1,#StartTimeTbl do
				local k = string.find(StartTimeTbl[j], ":")
				local hour = tonumber( string.sub(StartTimeTbl[j], 1, k-1) )
				local min = tonumber( string.sub(StartTimeTbl[j], k+1, -1) )
				local dif = (Date.hour*60+Date.min) - (hour*60+min)
				if dif>=0 and dif<=LastTime then
					return true
				end
			end
		end
	end
	return false
end


-------------------------------------------------------------------------------------

--------------玩家接受新任务时需要的数据库操作  begin

local function AddNewQuest(sQuestName,uCharId,TakeQestTime,uExpMultiple)
	if not uExpMultiple or not IsNumber(uExpMultiple) or uExpMultiple <= 0 then
		uExpMultiple = 1
	end
	StmtContainer._AddNewQuest:ExecSql("", sQuestName, uCharId,TakeQestTime,uExpMultiple)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

--添加新任务
function RoleQuestDB.AddNewQuest(sQuestName,uCharId,TakeQestTime,uExpMultiple)
	return AddNewQuest(sQuestName,uCharId,TakeQestTime,uExpMultiple)
end

local function AddNewQuestVar(sQuestName,uCharId,sVarName)
	StmtContainer._AddNewQuestVar:ExecSql("", sQuestName, uCharId, sVarName)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

--添加新任务的任务变量
function RoleQuestDB.AddNewQuestVar(sQuestName,uCharId,sVarName)
	return AddNewQuestVar(sQuestName,uCharId,sVarName)
end

local function ClearQuest(sQuestName,uCharId)
	StmtContainer._ClearQuest:ExecSql("", sQuestName, uCharId)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

--清空DB中的任务
function RoleQuestDB.ClearQuest(sQuestName,uCharId)
	return ClearQuest(sQuestName,uCharId)
end

local function DeleteAllQuestVar(sQuestName,uCharId)
	if #(RoleQuestDB.SelectQuestVarList(sQuestName,uCharId)) > 0 then
		StmtContainer._DeleteAllQuestVar:ExecSql("", sQuestName,uCharId)
		return g_DbChannelMgr:LastAffectedRowNum() > 0
	else
		return true
	end
end

--清空DB中任务的所有变量
function RoleQuestDB.DeleteAllQuestVar(sQuestName,uCharId)
	return DeleteAllQuestVar(sQuestName,uCharId)		
end
--------------玩家放弃任务时需要的数据库操作  end

local function GetQuestAcceptTime(sQuestName,uCharId)
	local ret = StmtContainer._GetQuestAcceptTime:ExecSql("n",sQuestName,uCharId)
	if(ret:GetRowNum() == 0) then
		ret:Release()
		return nil
	end
	local res = ret:GetData(0,0)
	ret:Release()
	return res
end

--获取任务开始时间
function RoleQuestDB.GetQuestAcceptTime(sQuestName,uCharId)
	return GetQuestAcceptTime(sQuestName,uCharId)
end

local function GetQuestState(data)
	local ret = StmtContainer._GetQuestState:ExecSql("n", data[1],data[2])
	if(ret:GetRowNum() == 0) then
		ret:Release()
		return nil
	end
	local res1,res2 = ret:GetData(0,0),ret:GetData(0,1)
	ret:Release()
	return res1,res2
end

--获取任务的状态
function RoleQuestDB.GetQuestState(data)
	return GetQuestState(data)
end

local function GetQuestStateFinishTime(char_id,QuestName)
	local ret = StmtContainer._GetQuestStateFinishTime:ExecSql("n", char_id,QuestName)
	if(ret:GetRowNum() == 0) then
		ret:Release()
		return nil
	end
	local res1,res2 = ret:GetData(0,0),ret:GetData(0,1)
	ret:Release()
	return res1,res2
end

local function UpdateQuestState(QuestState,finishTime,uCharId,sQuestName)
	StmtContainer._UpdateQuestState:ExecSql("", QuestState,finishTime,uCharId,sQuestName)
	return g_DbChannelMgr:LastAffectedRowNum() > 0 
end

--更新任务状态
function RoleQuestDB.UpdateQuestState(iQuestState,uCharId,sQuestName)
	local finishTime = 0
	if iQuestState == QuestState.finish then
		finishTime = os.time()
	end
	return UpdateQuestState(iQuestState,finishTime,uCharId,sQuestName)
end

local function UpdateQuestTime(TakeTime,uCharId,sQuestName)
	StmtContainer._UpdateQuestTime:ExecSql("", TakeTime,uCharId,sQuestName)
	return g_DbChannelMgr:LastAffectedRowNum() > 0 
end

--更新任务时间
function RoleQuestDB.UpdateQuestTime(TakeTime,uCharId,sQuestName)
	return UpdateQuestTime(TakeTime,uCharId,sQuestName)
end

local StmtDef = {
	"_UpdateQuestLimitTime",
	[[
		update
			tbl_quest
		set
			q_uLimitTime = 1
		where
			q_uLimitTime = 0 and cs_uId = ? and q_sName = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local function QuestIsLimitTime(uCharId,sQuestName)
	StmtContainer._UpdateQuestLimitTime:ExecSql("", uCharId,sQuestName)
	return g_DbChannelMgr:LastAffectedRowNum() > 0 
end

--更新任务的时间限制
function RoleQuestDB.QuestIsLimitTime(data)
	local uCharId,QuestName = data["PlayerId"],data["QuestName"]
	return QuestIsLimitTime(uCharId,QuestName)
end

local function InsertCharFinishQuestNum(uCharId,Num)
	StmtContainer._InsertCharFinishQuestNum:ExecSql("", uCharId,Num)
	return g_DbChannelMgr:LastAffectedRowNum() > 0 
end

--插入玩家完成任务数量
function RoleQuestDB.InsertCharFinishQuestNum(uCharId,Num)
	return InsertCharFinishQuestNum(uCharId,Num)
end

local function SelectCharFinishQuestNum(uCharId)
	local ret = StmtContainer._SelectCharFinishQuestNum:ExecSql("n", uCharId)
	local num = 0
	if(ret:GetRowNum() ~= 0) then
		num = ret:GetData(0,0)
	end
	ret:Release()
	return num
end

--查询玩家完成任务数量
function RoleQuestDB.SelectCharFinishQuestNum(uCharId)
	return SelectCharFinishQuestNum(uCharId)
end

local function UpdateCharFinishQuestNum(uCharId,AddNum)
	StmtContainer._AddCharFinishQuestNum:ExecSql("", AddNum,uCharId)
	return g_DbChannelMgr:LastAffectedRowNum() > 0 
end

--插入玩家完成任务数量
function RoleQuestDB.UpdateCharFinishQuestNum(uCharId,AddNum)
	return UpdateCharFinishQuestNum(uCharId,AddNum)
end

local function GetQuestExpMultiple(uCharId,sQuestName)
	local ret = StmtContainer._GetQuestExpMultiple:ExecSql("n", uCharId,sQuestName)
	if(ret:GetRowNum() == 0) then
		ret:Release()
		return 1
	end
	local res = ret:GetData(0,0)
	ret:Release()
	return res
end

--获取任务奖励的倍数
function RoleQuestDB.GetQuestExpMultiple(uCharId,sQuestName)
	return GetQuestExpMultiple(uCharId,sQuestName)
end

local function SelectQuestVarNum(sQuestName,sVarName,uCharId)
	local ret = StmtContainer._SelectQuestVarNum:ExecSql("n", sQuestName, sVarName, uCharId)
	if(ret:GetRowNum() == 0) then
		ret:Release()
		return nil
	end
	local res = ret:GetData(0,0)
	ret:Release()
	return res
end

local function CheckQuestVarNeedNum(data)
	local sQuestName = data["sQuestName"]
	local sVarName = data["sVarName"]
	local uCharId = data["uCharId"]
	if g_QuestNeedMgr[sQuestName] then
		local NeedNum = g_QuestNeedMgr[sQuestName][sVarName].Num
		local DoNum = SelectQuestVarNum(sQuestName, sVarName, uCharId)
		if DoNum and DoNum < NeedNum then
			return NeedNum-DoNum
		end
	end
	return nil
end

--获取指定任务变量数目
function RoleQuestDB.CheckQuestVarNeedNum(data)
	return CheckQuestVarNeedNum(data)
end

local function CheckQuestItemNeedNum(data)
	local sQuestName = data["sQuestName"]
	local iItemType = data["iItemType"]
	local sItemName = data["sItemName"]
	local uCharId = data["uCharId"]
	local NeedNum = g_QuestNeedMgr[sQuestName][sItemName].Num
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local iState = RoleQuestDB.GetQuestState({uCharId,sQuestName})
	if iState ~= QuestState.init then
		return nil
	end
	local HaveNum = g_RoomMgr.GetItemCount(uCharId, iItemType, sItemName)
	--local HaveNum = SelectQuestVarNum(sQuestName, sVarName, uCharId)
	if HaveNum and HaveNum < NeedNum then
		return NeedNum-HaveNum
	end
	return nil
end

--获取任务物品需要的变量数目
function RoleQuestDB.CheckQuestItemNeedNum(data)
	return CheckQuestItemNeedNum(data)
end

local function CheckQuestSatisfy(data)
	local sQuestName = data["sQuestName"]
	local uCharId = data["uCharId"]
	
	local iState = RoleQuestDB.GetQuestState({uCharId,sQuestName})
	if iState ~= QuestState.init then
		return false
	end
	
	local vartbl = RoleQuestDB.SelectQuestVarList(sQuestName,uCharId)
	local num1 = table.getn(vartbl)
	for i = 1, num1 do
		local sVarName = vartbl[i][1]
		local DoNum = vartbl[i][2]
		local NeedNum = g_QuestNeedMgr[sQuestName][sVarName].Num
		if DoNum < NeedNum then
			return true
		end
	end
	
	if Quest_Common(sQuestName, "物品需求") then
		local Keys = Quest_Common:GetKeys(sQuestName, "物品需求")
		for k = 1, table.getn(Keys) do
			local Arg = GetCfgTransformValue(true, "Quest_Common", sQuestName, "物品需求", Keys[k], "Arg")
			local needtype = Arg[1]
			local needname = Arg[2]		--需求的物品名
			local neednum = Arg[3]
			local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
			local have_num = g_RoomMgr.GetItemCount(uCharId,needtype,needname)
			if have_num < neednum then
				return true
			end
		end
	end
	return false
end

--检测任务是否存在,并处于没有完成的状态
function RoleQuestDB.CheckQuestSatisfy(data)
	return CheckQuestSatisfy(data)
end

local function GetDoingQuestSql(uCharId)
	local DbQuestAll = {}
	local Ret = StmtContainer._GetDoingQuests:ExecStat(uCharId)
	for i=1, Ret:GetRowNum() do
		local questname = Ret:GetData(i-1, 0)
		if Quest_Common(questname) then
			table.insert(DbQuestAll, {questname})
		end
	end
	Ret:Release()
	
	return DbQuestAll
end

--得到某个玩家的所有任务
function RoleQuestDB.GetDoingQuestSql(uCharId)
	return GetDoingQuestSql(uCharId)
end

local StmtDef = {
	"_InsertAwardItemIndex",
	[[
		replace into tbl_quest_tempitem (q_sName,cs_uId,qt_ItemIndex) values(?,?,?)
	]]
}
DefineSql( StmtDef, StmtContainer )
--设置这个任务可以得到的物品ID(佣兵挑战任务用)
local ItemLevelTbl = {
	[1]={1,30},
	[2]={31,40},
	[3]={41,50},
	[4]={51,60},
	[5]={61,70},
	[6]={71,80},
}
local function InsertAwardItemIndex(charId, QuestName)
	if g_AllMercenaryQuestMgr[QuestName] then
		local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
		local Level = CharacterMediatorDB.GetPlayerLevel(charId)
		local ItemIndex = nil
		for i=1, #(ItemLevelTbl) do
			if ItemLevelTbl[i][1] <= Level and ItemLevelTbl[i][2] >= Level then
				ItemIndex = i
				break
			end
		end
		
		local AwardItem = GetCfgTransformValue(true, "Quest_Common", QuestName, "物品奖励", "Subject")
		if Quest_Common(QuestName, "物品奖励") and AwardItem[1] then
			if AwardItem[ItemIndex] then
				StmtContainer._InsertAwardItemIndex:ExecSql("", QuestName, charId, ItemIndex)
				return g_DbChannelMgr:LastAffectedRowNum() > 0, ItemIndex
			end
		end
	end
	return true, 0
end

local StmtDef = {
	"_GetAwardItemFromQuest",
	[[
		select
			qt_ItemIndex
		from
			tbl_quest_tempitem
		where
			cs_uId = ? and q_sName = ?
	]]
}
DefineSql( StmtDef, StmtContainer )
--得到任务可得的物品ID(佣兵挑战任务用)
local function GetAwardItemFromQuest(charId, QuestName)
	local ret = StmtContainer._GetAwardItemFromQuest:ExecSql("n", charId, QuestName)
	if(ret:GetRowNum() == 0) then
		ret:Release()
		return
	end
	local res = ret:GetData(0,0)
	ret:Release()
	return res
end

function RoleQuestDB.GetAwardItemFromQuest(data)
	local charId = data["char_Id"]
	local QuestName = data["QuestName"]
	return GetAwardItemFromQuest(charId, QuestName)
end

local function CheckQuestIsInArea(QuestName,AreaName)
	if not Quest_Common(QuestName, "所属区域") then
		return false
	end
	local Keys = Quest_Common:GetKeys(QuestName, "所属区域")
	for n = 1, table.getn(Keys) do
		if AreaName == Quest_Common(QuestName, "所属区域", Keys[n]) then
			return true
		end
	end
	return false
end

local function CheckQuestIsKillNpc(QuestName)
	if Quest_Common(QuestName, "杀怪需求") then
		return false
	end
	return true
end

local function Is2CharHaveSameKillNpcQuest(AreaName,uCharId1,uCharId2)
	if g_AreaQuestMgr[AreaName] == nil then
		return false
	end
	local ret1 = StmtContainer._GetDoingQuests:ExecSql("s[32]", uCharId1)
	local num1 = ret1:GetRowNum()
	if num1 == 0 then
		ret1:Release()
		return false
	end
	
	local ret2 = StmtContainer._GetDoingQuests:ExecSql("s[32]", uCharId2)
	local num2 = ret2:GetRowNum()
	if num2 == 0 then
		ret2:Release()
		return false
	end
	
	for i = 1, num1 do
		local QuestName1 = ret1:GetData(i-1,0)
		for j = 1,num2 do
			local QuestName2 = ret2:GetData(j-1,0)
			if QuestName1 == QuestName2 
				and CheckQuestIsKillNpc(QuestName1)
				and CheckQuestIsInArea(QuestName1,AreaName) then
				
				return true
			end
		end
	end
	ret1:Release()
	ret2:Release()
	return false
end

function RoleQuestDB.Is2CharHaveSameKillNpcQuest(AreaName,uCharId1,uCharId2)
	return Is2CharHaveSameKillNpcQuest(AreaName,uCharId1,uCharId2)
end

--获取当前任务名称，变量，数目
local function SelectQuestVarList(sQuestName,uCharId)
	local tbl = {}
	local ret = StmtContainer._SelectQNameVNameVNum:ExecSql("s[128]n", sQuestName,uCharId)
	for i = 1, ret:GetRowNum() do
		tbl[i] = {ret:GetData(i-1,0),ret:GetData(i-1,1)}
	end
	ret:Release()
	return tbl
end

--获取当前任务名称，变量，数目
function RoleQuestDB.SelectQuestVarList(sQuestName,uCharId)
	return SelectQuestVarList(sQuestName,uCharId)
end

local function AddQuestVarNum(data)
	local iNum = data["iNum"]
	local sQuestName = data["sQuestName"]
	local sVarName = data["sVarName"]
	local uCharId = data["char_id"]
	local NeedDoNum = g_QuestNeedMgr[sQuestName][sVarName].Num
	StmtContainer._AddQuestVarNum:ExecSql("", iNum, uCharId, sQuestName, sVarName, iNum, NeedDoNum)
--	print("RoleQuestDB.AddQuestVarNum,g_DbChannelMgr:LastAffectedRowNum() > 0",g_DbChannelMgr:LastAffectedRowNum() > 0)
	local IsFinish = g_DbChannelMgr:LastAffectedRowNum() > 0
	return IsFinish
end

--更新已知任务和已知变量的数量
function RoleQuestDB.AddQuestVarNum(data)
	return AddQuestVarNum(data)
end

local function AddVarNumForQuest(sQuestName,sVarName,uCharId,iNum)
	local NeedDoNum = g_QuestNeedMgr[sQuestName][sVarName].Num
	local DoNum = SelectQuestVarNum(sQuestName,sVarName,uCharId)
	if DoNum ~= nil and DoNum < NeedDoNum then
		local ResNum = DoNum+iNum
		if DoNum+iNum > NeedDoNum then
			iNum = NeedDoNum - DoNum
			ResNum = NeedDoNum
		end
		StmtContainer._SetQuestVarNum:ExecSql("", ResNum, uCharId, sQuestName, sVarName)
		if g_DbChannelMgr:LastAffectedRowNum() > 0 then
			return iNum
		else
			return false
		end
	end
	return nil
end

local function AddVarNumForTeamQuests(data)
	local uCharId = data["char_id"]
	local sVarName = data["sVarName"]
	local iNum = data["iNum"]
	local CanShareTeamMateTbl = data["CanShareTeamMateTbl"]
	
	local PlayerAddQuestVarTbl = {}
	for i = 1,table.getn(g_VarQuestMgr[sVarName]) do
		local sQuestName = g_VarQuestMgr[sVarName][i]
		local ResNum = AddVarNumForQuest(sQuestName,sVarName,uCharId,iNum)
		if ResNum ~= nil then
			if PlayerAddQuestVarTbl[uCharId] == nil then
				PlayerAddQuestVarTbl[uCharId] = {}
			end
			table.insert(PlayerAddQuestVarTbl[uCharId],{sQuestName,sVarName,ResNum})
		end
	end
	
	for i = 1,table.getn(CanShareTeamMateTbl) do
		data["char_id"] = CanShareTeamMateTbl[i]
		for j = 1,table.getn(g_VarQuestMgr[sVarName]) do
			local sQuestName = g_VarQuestMgr[sVarName][j]
			local bNotShare = g_QuestNeedMgr[sQuestName][sVarName].bNotShare
			if not bNotShare then
				local ResNum = AddVarNumForQuest(sQuestName,sVarName,CanShareTeamMateTbl[i],iNum)
				if ResNum then
					if PlayerAddQuestVarTbl[CanShareTeamMateTbl[i]] == nil then
						PlayerAddQuestVarTbl[CanShareTeamMateTbl[i]] = {}
					end
					table.insert(PlayerAddQuestVarTbl[CanShareTeamMateTbl[i]],{sQuestName,sVarName,ResNum})
				end
			end
		end
	end
	return PlayerAddQuestVarTbl
end

--增加队队伍有需求名为（sVarName）的所有任务的计数
function RoleQuestDB.AddVarNumForTeamQuests(data)
	return AddVarNumForTeamQuests(data)
end

local function LevelQuestFail(data)
	local charId = data["char_id"]
	local Level = data["Level"]
	local FailQuestTbl = {}
	local QuestNumRes = StmtContainer._GetDoingQuests:ExecStat(charId)
	if QuestNumRes:GetRowNum() > 0 then
		for i=1, QuestNumRes:GetRowNum() do
			local sQuestName = QuestNumRes:GetData(i-1,0)
			if Quest_Common(sQuestName) then
				local MaxLevel = Quest_Common(sQuestName, "完成任务的最高等级")
				if MaxLevel and MaxLevel~=0 and Level > MaxLevel then
					local param = {}
					param["PlayerId"] = charId
					param["QuestName"] = sQuestName
					table.insert(FailQuestTbl,{RoleQuestDB.QuestFailure(param),sQuestName})
				end
			end
		end
		QuestNumRes:Release()
	end
	return FailQuestTbl
end

--增加跟等级有关的任务计数,和让超过等级的任务失败
function RoleQuestDB.AddVarNumForLevelQuests(data)
	local param = {}
	if data["sVarName"] then
		param["PlayerAddQuestInfo"] = AddVarNumForTeamQuests(data)
	end
	param["PlayerFailQuestInfo"] = LevelQuestFail(data)
	return param
end

local function AddQuestVarNumForTeam(data)
	local tbl = {}
	tbl[data["char_id"]] = RoleQuestDB.AddQuestVarNum(data)
	local CanShareTeamMateTbl = data["CanShareTeamMateTbl"]
	for i = 1, #(CanShareTeamMateTbl) do
		data["char_id"] = CanShareTeamMateTbl[i]
		tbl[CanShareTeamMateTbl[i]] = RoleQuestDB.AddQuestVarNum(data)
	end
	return tbl
end

-- 增加队伍指定任务sQuestName的指定需求sVarName的计数
function RoleQuestDB.AddQuestVarNumForTeam(data)
	return AddQuestVarNumForTeam(data)
end 

local function SetQuestVarNum(data)
	local sQuestName = data["sQuestName"]
	local sVarName = data["sVarName"]
	local DoNum = data["DoNum"]
	local uCharId = data["char_id"]
	StmtContainer._SetQuestVarNum:ExecSql("", DoNum + 1, uCharId, sQuestName, sVarName)
	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
		return DoNum + 1
	else
		return nil
	end
	return nil
end

--设置任务变量数
function RoleQuestDB.SetQuestVarNum(data)
	return SetQuestVarNum(data)
end

--计算佣兵积分值
local function CalculateSingleIntegral(MonsterLevel,CharacterLevel)
	local uLevelDiff = MonsterLevel - CharacterLevel;
	local Exp=0

	if uLevelDiff <= -20 then
		return Exp
	end

	if uLevelDiff > 5 then
		uLevelDiff = 5
	end

	Exp = ((CharacterLevel*3)+50) * (1 + uLevelDiff*4 / (CharacterLevel+60))
	return Exp
end

--杀怪后,经验和等级的变化
local function AddPlayerExp(PlayerLevel,AddExpNum,DBExp,PlayerId)
	--佣兵等级限制人物等级、加经验系数
	--local MercenaryLevelDB = RequireDbBox("MercenaryLevelDB")
	--local data = {}
	--data["CharId"] = PlayerId
	--local MercenaryLevel = MercenaryLevelDB.GetMercenaryLevelInfo(data)
	--local FinishAwards = MercenaryLevelDB.GetMercenaryLevelAward(data)
	--MercenaryLevel = MercenaryLevel or 0
	--local LimitLevel = nil
	--if FinishAwards["开放等级"] then
	--	if g_MercenaryLevelTbl[MercenaryLevel]["特权"]["开放等级"] then
	--		LimitLevel = g_MercenaryLevelTbl[MercenaryLevel]["特权"]["开放等级"].Arg
	--	else
	--		LimitLevel = g_TestRevisionMaxLevel
	--	end
	--else
	--	if MercenaryLevel==0 or g_MercenaryLevelTbl[MercenaryLevel-1]["特权"]["开放等级"] then
	--		LimitLevel = (MercenaryLevel==0 and 10) or g_MercenaryLevelTbl[MercenaryLevel-1]["特权"]["开放等级"].Arg
	--	else
	--		LimitLevel = g_TestRevisionMaxLevel
	--	end
	--end
	--if FinishAwards["经验提升"] then
	--	AddExpNum = AddExpNum * (1+ g_MercenaryLevelTbl[MercenaryLevel]["特权"]["经验提升"].Arg)
	--elseif MercenaryLevel~=0 and g_MercenaryLevelTbl[MercenaryLevel-1]["特权"]["经验提升"] then
	--	AddExpNum = AddExpNum * (1+ g_MercenaryLevelTbl[MercenaryLevel-1]["特权"]["经验提升"].Arg)
	--end
	
	local LevelExp = DBExp + AddExpNum
	local nCurlevel = PlayerLevel
	local LimitLevel = g_TestRevisionMaxLevel	--math.min(LimitLevel, g_TestRevisionMaxLevel)
	if nCurlevel == LimitLevel then
		return nil
	end
	if nCurlevel == LevelExp_Common.GetHeight() then
		return nil
	end
	
	while LevelExp >= LevelExp_Common(nCurlevel, "ExpOfCurLevelUp") do
		--如果不能升级，当前经验为0
		if LevelExp_Common(nCurlevel, "ExpOfCurLevelUp") == 0 then
			LevelExp = 0
			break
		end
		--减去升级经验，并且提升一级
		LevelExp = LevelExp - LevelExp_Common(nCurlevel, "ExpOfCurLevelUp")

		nCurlevel = nCurlevel + 1
		if nCurlevel >= LimitLevel then
			LevelExp = 0
			break
		end
		if nCurlevel >= LevelExp_Common.GetHeight() then
			LevelExp = 0
			break
		end
	end
	return nCurlevel,LevelExp
end

--佣兵任务完成后，加佣兵积分
local function AddPlayerMercenaryIntegral(PlayerMercenaryLevel,AddIntegral,DBIntegral)
	local LevelIntegral = DBIntegral + AddIntegral
	local Currlevel = PlayerMercenaryLevel

	--if Currlevel == #(MercenaryLevel_Common) then
	--	return nil
	--end
	
--	local iQuestLevel = 1
--	for i=1, #(MercenaryLevel_Common) do
--		if MercenaryLevel_Common[i].MercenaryLevel == QuestLevel then
--			iQuestLevel = i
--			break
--		end
--	end
--	
--	if iQuestLevel > PlayerMercenaryLevel then
--		return iQuestLevel,0
--	end
--	
--	while LevelIntegral >= MercenaryLevel_Common[Currlevel].IntegralOfCurLevelUp do
--		--如果不能升级，当前经验为0
--		if MercenaryLevel_Common[Currlevel].IntegralOfCurLevelUp == 0 
--			or MercenaryLevel_Common[Currlevel].IntegralOfCurLevelUp == "" then
--			LevelIntegral = 0
--			break
--		end
--		--减去升级经验，并且提升一级
--		LevelIntegral = LevelIntegral - MercenaryLevel_Common[Currlevel].IntegralOfCurLevelUp
--
--		Currlevel = Currlevel + 1
--		if Currlevel >= #(MercenaryLevel_Common) then
--			LevelIntegral = 0
--			break
--		end
--	end
	return Currlevel,LevelIntegral
end

--查询队友表  QuestName 任务名称    任务是否可重复
--返回可以接受任务的对友 tbl
--yy
function RoleQuestDB.QuestTeamPlayer(data)
	local TeamID = data["TeamID"]
	local QuestName = data["HideQuestName"]
	local reTbl = {}
	--if TeamID ~= 0 then
	local team = RequireDbBox("GasTeamDB")
	local tblMembers = team.GetTeamMembers(TeamID)

	for k = 1, #(tblMembers)do
--		print("tblMembers[k]",tblMembers[k])
		local state=RoleQuestDB.GetQuestState({tblMembers[k][1], QuestName})
		if state ~= nil then
			if state == QuestState.finish then
				if g_RepeatQuestMgr[QuestName] then
					table.insert( reTbl, tblMembers[k][1])
				end
			end
		else
			table.insert( reTbl, tblMembers[k][1])
		end
	end
	return reTbl
end	

--杀死怪后增加玩家的任务记数
local function AddPlayerKillNpcVarNum(data)
	local NpcName = data["NpcName"]
	local ShowName = data["ShowName"]
	local PlayerId = data["char_id"]
	
	if data["QuestNpc"][PlayerId] then
		local isAddQuestNum = false
		for _,Tbl in pairs(data["QuestNpc"][PlayerId]) do
			for i=1, #(Tbl) do
				if Tbl[i] == NpcName or Tbl[i] == ShowName then
					isAddQuestNum = true
					break
				end
			end
			if isAddQuestNum then
				break
			end
		end
		
		if isAddQuestNum then
			
			local statetbl = data["DeadStateTbl"]
			
			local RecordName = {} 
			if g_KillNpcQuestMgr[NpcName] then
				RecordName[1] = NpcName
			end
			if g_KillNpcQuestMgr[ShowName] then
				RecordName[2] = ShowName
			end
			
			local QuestTbl = {}
			for index = 1, 2 do
				if RecordName[index] and statetbl[index] then
					for i = 1, table.getn(statetbl[index]) do
						local statename = statetbl[index][i]
						local varName = statename.."杀"..RecordName[index]
						if g_KillNpcQuestMgr[RecordName[index]][statename] then
							for j = 1, table.getn(g_KillNpcQuestMgr[RecordName[index]][statename]) do
								local questname = g_KillNpcQuestMgr[RecordName[index]][statename][j]
								data["sQuestName"] = questname
								data["sVarName"] = varName
								data["iNum"] = 1
								local IsTure = RoleQuestDB.AddQuestVarNum(data)
								if IsTure then
									local cQuestTbl = {}
									cQuestTbl["QuestName"] = data["sQuestName"]
									cQuestTbl["VarName"] = varName
									cQuestTbl["AddNum"] = 1
									table.insert(QuestTbl,cQuestTbl)
								end
							end
						end
					end
					
				end
			end
			return QuestTbl
			
		end
	end
end

--杀怪后,添加任务相关的信息和提升等级,如果有掉落,则产生掉落物品的tal表
--掉落物品
--(注意:如果直接在CallDbTrans中调用此方法会返回4个参数,相当于是,正常的掉落方式)
--(注意:如果是用到传奇掉落,用RoleQuestDB.KillNpcAddQuestDropObj()调用此方法会返回5个参数)
local function KillNpcAddQuestDropObj(data)
	local AddQuestResTbl = {}
	local PlayerId = data["char_id"]
	
	local CanShareTeamMateTbl = data["TeamMemberID"]
	--local TeamMemberID = data["TeamMemberID"]
	--local AreaDB = RequireDbBox("AreaDB")
	
--	for i=1, #(TeamMemberID) do
--		local teammateid = TeamMemberID[i]
--		if AreaDB.Check2PlayerInSameArea(PlayerId,teammateid) then
--			table.insert(CanShareTeamMateTbl,teammateid)
--		end
--	end
	
	--第一个人的任务记数
	local AddQuestRes = AddPlayerKillNpcVarNum(data)
	if AddQuestRes and next(AddQuestRes) then
		table.insert(AddQuestResTbl,{PlayerId,AddQuestRes})
	end
	
	for i=1, table.getn(CanShareTeamMateTbl) do		
		data["char_id"] = CanShareTeamMateTbl[i]
		
		AddQuestRes = AddPlayerKillNpcVarNum(data)
		if AddQuestRes and next(AddQuestRes) then
			table.insert(AddQuestResTbl,{CanShareTeamMateTbl[i],AddQuestRes})
		end
		
	end
	
	local NpcDropItemDB = RequireDbBox("NpcDropItemDB")
	local DropObjTbl,DropItemTbl,AddItemResTbl = NpcDropItemDB.NpcDeadDrop(PlayerId,data,CanShareTeamMateTbl,data["sceneName"])
	if data["IsBoss"] then
		local LogMgr = RequireDbBox("LogMgrDB")
		LogMgr.SaveNpcDeadInfo( PlayerId, data["NpcName"],data["PlayerLevel"])
	end
	return {AddQuestResTbl,AddItemResTbl,DropItemTbl,DropObjTbl}
end

function RoleQuestDB.KillNpcAddQuestDropObj(data)
	return KillNpcAddQuestDropObj(data)
end

--添加可以触发任务的剧场
function RoleQuestDB.AddQuestTheater(data)
	local PlayerTbl = data["PlayerTbl"]
	local TheaterName = data["TheaterName"]
	local PlayerQuestTbl = {}
	if g_VarQuestMgr[TheaterName] == nil then
		return PlayerQuestTbl
	end
	
	data["sVarName"] = TheaterName
	data["iNum"] = 1
	for i = 1, table.getn(PlayerTbl) do
		data["char_id"] = PlayerTbl[i]
		for j = 1, table.getn(g_VarQuestMgr[TheaterName]) do
			data["sQuestName"] = g_VarQuestMgr[TheaterName][j]
			local IsTrue = RoleQuestDB.AddQuestVarNum(data)
			if IsTrue then
				PlayerQuestTbl[PlayerTbl[i]] = {}
				PlayerQuestTbl[PlayerTbl[i]]["questname"] = g_VarQuestMgr[TheaterName][j]
				PlayerQuestTbl[PlayerTbl[i]]["VarName"] = TheaterName
				PlayerQuestTbl[PlayerTbl[i]]["AddNum"] = 1
			end
		end
	end
	return PlayerQuestTbl
end

--清空该任务某个任务变量
function RoleQuestDB.DeleteQuestVar(sQuestName, uCharId, sVarName)
	StmtContainer._DeleteQuestVar:ExecSql("", sQuestName, uCharId, sVarName)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

--放弃任务
function RoleQuestDB.GiveUpQuestSql(sQuestName, uCharId)
	StmtContainer._GiveUpQuestSql:ExecSql("", sQuestName, uCharId)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

local function GetAllQuestVar(uCharId)
	local vartbl = {}
	local delQuestTbl = {}
	local res = StmtContainer._GetAllQuestVar:ExecSql("s[32]s[128]n", uCharId)
 	for i = 1, res:GetRowNum() do
 		local questname = res:GetData(i-1, 0)
 		local varname = res:GetData(i-1, 1)
 		local varnum = res:GetData(i-1, 2)
 		if g_QuestNeedMgr[questname] and g_QuestNeedMgr[questname][varname] then
	 		if vartbl[questname] == nil then
				vartbl[questname] = {}
			end
	 		table.insert(vartbl[questname],{varname, varnum})
	 	else
	 		table.insert(delQuestTbl,questname)
	 	end
 	end
 	res:Release()
 	
 	for i=1, #(delQuestTbl) do
 		StmtContainer._GiveUpQuestSql:ExecStat(delQuestTbl[i], uCharId)
 	end
 	
 	return vartbl
end

--获取某任务的所有任务变量 参数
function RoleQuestDB.GetAllQuestVar(uCharId)
	return GetAllQuestVar(uCharId)
end

local function GetAllQuestSql(uCharId)
	local DbQuestAll = {}
	local delQuestTbl = {}
	local Ret = StmtContainer._GetAllQuestSql:ExecStat(uCharId)
	for i=1, Ret:GetRowNum() do
		local questname = Ret:GetData(i-1, 0)
		if Quest_Common(questname) then
			table.insert(DbQuestAll, {questname, Ret:GetData(i-1, 1), Ret:GetData(i-1, 2), Ret:GetData(i-1, 3)})
		else
			table.insert(delQuestTbl,questname)
		end
	end
	Ret:Release()
	
	--如果策划已经去掉这个任务了,就从数据库中把它删除掉
	for i=1, #(delQuestTbl) do
		StmtContainer._GiveUpQuestSql:ExecStat(delQuestTbl[i], uCharId)
	end
	
	return DbQuestAll
end

--得到某个玩家的所有任务
function RoleQuestDB.GetAllQuestSql(uCharId)
	return GetAllQuestSql(uCharId)
end

local StmtDef = {
	"_GetAllAwardItemFromQuest",
	[[
		select
			q_sName,qt_ItemIndex
		from
			tbl_quest_tempitem
		where
			cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

--得到所有佣兵任务,接取时,要奖励给玩家的物品
local function GetAllQuestAwardItemSql(uCharId)
	local QusetAwardItem = {}
	local Ret = StmtContainer._GetAllAwardItemFromQuest:ExecStat(uCharId)
	for i=1, Ret:GetRowNum() do
		local QuestName = Ret:GetData(i-1, 0)
		local AwardItemIndex = Ret:GetData(i-1, 1)
		QusetAwardItem[QuestName] = AwardItemIndex
	end
	Ret:Release()
	return QusetAwardItem
end

function RoleQuestDB.GetAllQuestAwardItemSql(uCharId)
	return GetAllQuestAwardItemSql(uCharId)
end

local StmtDef = {
	"_GetAllFinishQuest",
	[[
		select q_sName, q_tAcceptTime, q_tFinishTime from tbl_quest where q_state = 3 and cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local function GetAllYbFinishQuest(uCharId)
	local DbQuestAll = {}
	local Ret = StmtContainer._GetAllFinishQuest:ExecSql("s[32]nn",uCharId)
	for i=1, Ret:GetRowNum() do
		local QuestName = Ret:GetData(i-1, 0)
		if g_AllMercenaryQuestMgr[QuestName] then--是佣兵任务
			DbQuestAll[QuestName] = {Ret:GetData(i-1, 1), Ret:GetData(i-1, 2)}
		end
	end
	Ret:Release()
	return DbQuestAll
end

function RoleQuestDB.GetAllYbFinishQuest(data)
	local uCharId = data["PlayerId"]
	return GetAllYbFinishQuest(uCharId)
end

local function GetNowQuestSql(uCharId)
	local DbNowQuest = {}
	local res = StmtContainer._GetNowQuestSql:ExecSql("s[32]s[128]n", uCharId)
 	for i = 1, res:GetRowNum() do
 		local questname = res:GetData(i-1, 0)
 		DbNowQuest[questname] = DbNowQuest[questname] or {}
 		local varname, varnum = res:GetData(i-1, 1), res:GetData(i-1, 2)
 		DbNowQuest[questname][varname] = varnum
 	end
 	res:Release()
 	return DbNowQuest
end

--得到玩家正在进行中的任务
function RoleQuestDB.GetNowQuestSql(uCharId)
	return GetNowQuestSql(uCharId)
end

local function GetItemVarNum(PlayerId,sQuestName,ItemType, ItemName)
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local inbagnum = g_RoomMgr.GetItemCount(PlayerId, ItemType, ItemName)
	local neednum = g_QuestNeedMgr[sQuestName][ItemName].Num
	local ItemTbl = {}
	ItemTbl["QuestName"] = sQuestName
	ItemTbl["ItemType"] = ItemType
	ItemTbl["ItemName"] = ItemName
	if inbagnum > neednum then
		ItemTbl["DoNum"] = neednum
	else
		ItemTbl["DoNum"] = inbagnum
	end
	StmtContainer._SetQuestVarNum:ExecSql("", ItemTbl["DoNum"], PlayerId, sQuestName, ItemName)
--	print("StmtContainer._SetQuestVarNum,g_DbChannelMgr:LastAffectedRowNum() > 0",g_DbChannelMgr:LastAffectedRowNum() > 0)
	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
		return ItemTbl
	else
		return nil
	end
end

local function check_quest_var_full(sVarName, sQuestName, nCount)
	--[[
	查看任务物品是否已经满了 参数为变量名，任务名，现有物品数目
	--]]
	for varname, p in pairs(g_QuestNeedMgr[sQuestName]) do
		if varname == sVarName then
			return p.Num == nCount
		end
	end
	return false
end

local function IncreQuestItemVar(PlayerId,ItemType, ItemName, nCount)
	local QuestItemTbl = {}
	if g_ItemQuestMgr[ItemName] then
		for i = 1, table.getn(g_ItemQuestMgr[ItemName]) do
			if ItemType == g_ItemQuestMgr[ItemName][i][1] then
				local sQuestName = g_ItemQuestMgr[ItemName][i][2]
				local ItemTbl = GetItemVarNum(PlayerId,sQuestName, ItemType, ItemName)
				table.insert(QuestItemTbl,ItemTbl)
			end
		end
	end
	return QuestItemTbl
end

--local function add_item(PlayerId,ItemType, ItemName,nCount, res)
--	if not IsTable(res) then
--		return {}
--	end
--	return IncreQuestItemVar(PlayerId,ItemType, ItemName, nCount)
--end

----------------新加函数-----------------
local function CountReturn(DBLevel,PlayerYbLevel,LoopNum,str)
	str = string.gsub( str, "yblevel", PlayerYbLevel or 0)
	str = string.gsub( str, "level", DBLevel)
	str = string.gsub( str, "loop", LoopNum)
	local count = assert(loadstring( "return " .. str))()
	count = math.floor(count)
	return count
end

local function CountReturnSoulPearl(DBLevel,PlayerYbLevel,LoopNum,str)
	local i = string.find(str, ":")
	return string.sub(str, 1, i) .. CountReturn(DBLevel,PlayerYbLevel,LoopNum,string.sub(str, i+1))
end

--返回本任务的（经验值和魂值）
local function CountAddExpAndSoulNum(PlayerLevel,PlayerYbLevel,LoopNum,QuestName)
	local questExp   = 0
	local questSoul  = 0
	
	if Quest_Common(QuestName, "经验奖励") then
		local str = Quest_Common(QuestName, "经验奖励", "Subject")
		if str and str ~= "" then
			questExp = CountReturn(PlayerLevel,PlayerYbLevel,LoopNum,str)
		end
		
		str = Quest_Common(QuestName, "经验奖励", "Arg")
		if str and str ~= "" then
			questSoul = CountReturn(PlayerLevel,PlayerYbLevel,LoopNum,str)
		end
		
		if g_MasterStrokeQuestMgr[QuestName] then
			local str = Quest_Common(QuestName, "任务等级")
			local QuestLevel = CountReturn(PlayerLevel,PlayerYbLevel,LoopNum,str)
			QuestLevel = (QuestLevel~=0 and QuestLevel) or 1
			
			--当人物等级＝任务接取等级时，获得100%经验，不获得额外魂值
			--当人物等级＝任务接取等级＋1时，获得100%经验，不获得额外魂值
			
			if PlayerLevel - QuestLevel >= 3 then
				--当人物等级＝任务接取等级＋2时，获得50%经验，获得额外50%魂值
			--	questSoul = questSoul+Coefficient*questExp*0.5
				questExp = questExp*0.5
			--elseif PlayerLevel - QuestLevel >= 3 then
			--	--当人物等级≥任务接取等级＋3时，不获得经验，获得额外100%魂值
			--	questSoul = questSoul+Coefficient*questExp
			--	questExp = 0
			end
			
		end
	end
	questExp = math.floor(questExp)
	questSoul = math.floor(questSoul)
	return questExp,questSoul
end

--获得可以得到的佣兵积分值
local function GetMercenaryIntegralNum(PlayerLevel,PlayerYbLevel,LoopNum,QuestName)
	local questMercenaryIntegral = 0
	local strIntegral = Quest_Common(QuestName, "佣兵积分奖励")
	if strIntegral then
		if strIntegral ~= "" then
			questMercenaryIntegral = CountReturn(PlayerLevel,PlayerYbLevel,LoopNum,strIntegral)
		end
		
		--local MinLevel = nil
		--if questInfo["接任务最小佣兵等级"] and questInfo["接任务最小佣兵等级"][1].Subject then
		--	MinLevel = questInfo["接任务最小佣兵等级"][1].Subject
		--	for i=1, #(MercenaryLevel_Common) do
		--		if MercenaryLevel_Common[i].MercenaryLevel == MinLevel then
		--			MinLevel = i
		--			break
		--		end
		--	end
		--end
		
		--if MinLevel and (PlayerYbLevel - MinLevel) >= 2 then
		--	questMercenaryIntegral = 0
		--end
	end
	return questMercenaryIntegral
end

local function AddQuest(data)
	local PlayerId = data["PlayerId"]
	local sQuestName = data["QuestName"]
	local uExpMultiple = data["ExpMultiple"]
	local skillNeedQuestInfoTbl = data["skillNeedQuestInfoTbl"]
	local LoopNum = data["LoopNum"]
	
	--检测佣兵等级状态，完成全部基础达成条件后才能接
	--local MercenaryLevelDB = RequireDbBox("MercenaryLevelDB")
	--local param = {}
	--param["CharId"] = PlayerId
	--local MercenaryLevel, Status = MercenaryLevelDB.GetMercenaryLevelInfo(param)
	--if MercenaryLevel and MercenaryLevel<g_MercenaryLevelTbl.MaxLevel then
	--	local tbl = g_MercenaryLevelTbl[MercenaryLevel]["挑战条件"]
	--	if tbl.Subject=="挑战试炼" and ( sQuestName==tbl.Arg[1] or sQuestName==tbl.Arg[2] or sQuestName==tbl.Arg[3] ) then
	--		if Status ~= 2 then
	--			CancelTran()
	--			return {false,"未完成全部基础达成条件"}
	--		end
	--	end
	--end
	
	local state=RoleQuestDB.GetQuestState({PlayerId, sQuestName})
	if(state~=nil) then
		--任务已经接取过
		if state==QuestState.finish then
			if g_RepeatQuestMgr[sQuestName] then
			--判断是否已经完成该任务，是否是可重复任务
				if not RoleQuestDB.UpdateQuestTime(os.time(),PlayerId,sQuestName) then
					CancelTran()
					return {false,3075} --"更新任务时间失败"
				end
				if not RoleQuestDB.UpdateQuestState(QuestState.init,PlayerId,sQuestName) then
					CancelTran()
					return {false,3076} --"更新任务状态失败"
				end
			else
				CancelTran()
				return {false,3077} --"任务不可重复"
			end
		elseif state==QuestState.failure then
				--任务已经失败，重新接取，更新任务状态
			if not RoleQuestDB.UpdateQuestState(QuestState.init,PlayerId,sQuestName) then
				CancelTran()
				return {false,3076} --"更新任务状态失败"
			end
		else
			CancelTran()
			return {false, 3078} --"任务已经接取"
		end
	else
		if not RoleQuestDB.AddNewQuest(sQuestName,PlayerId,os.time(),uExpMultiple) then
			CancelTran()
			return {false,3079} --"添加任务失败"
		end
	end		
	
	--增加任务变量
	if g_QuestNeedMgr[sQuestName] then
		for i,p in pairs(g_QuestNeedMgr[sQuestName]) do
			if p.NeedType ~= "物品需求" and not RoleQuestDB.AddNewQuestVar(sQuestName,PlayerId,i) then
				CancelTran()
				return {false,3080} --"添加任务变量失败"
			end
		end
	end
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
		--给予任务道具
	local give_items={}
	if Quest_Common(sQuestName, "获得物品") then
		local Keys = Quest_Common:GetKeys(sQuestName, "获得物品")
		for i = 1,table.getn(Keys) do
			local tbl = GetCfgTransformValue(true, "Quest_Common", sQuestName, "获得物品", Keys[i])
			if tbl[3] then
				local give_type = tbl[1]
				local give_name = tbl[2]
				local give_num = tonumber(tbl[3])
				local add_num = give_num-g_RoomMgr.GetItemCount(PlayerId, give_type, give_name)
				if(add_num>0) then
					local params= {}
					params.nCharID = PlayerId
					params.nStoreMain = g_StoreRoomIndex.PlayerBag
					params.nBigID = give_type
					params.nIndex = give_name
					params.nCount = add_num
					params.createType = event_type_tbl["任务品或者道具给予"]
					local res = g_RoomMgr.AddItem(params)
					if IsNumber(res) then
						CancelTran()
						return {false,res}
					end
					table.insert(give_items,{give_type,give_name,add_num, res})
				end
			end
		end
	end
	
	--存要得到的佣兵物品ID
	local IsSucc, YbItemIndex = InsertAwardItemIndex(PlayerId, sQuestName)
	if not IsSucc then
		CancelTran()
		return {false, 3082} --"数据存储佣兵任务物品失败"
	end
	
	--若是含有转职事件需求的任务，判断是否已经转职，已经转职的话直接加计数
	local QuestVarTbl = {}
	if g_VarQuestMgr["完成转职"] then
		for i=0,#g_VarQuestMgr["完成转职"] do
			if g_VarQuestMgr["完成转职"][i] == sQuestName then
				local FightSkillDB = RequireDbBox("FightSkillDB")
				local IsSeries = FightSkillDB.IsSeries(PlayerId)
				if IsSeries then
					local param = {}
					param["char_id"] = PlayerId
					param["sVarName"] = "完成转职"
					param["iNum"] = 1
					param["CanShareTeamMateTbl"] = {}
					table.insert(QuestVarTbl, AddVarNumForTeamQuests(param))
					if QuestVarTbl[1][PlayerId][1][3] == false then
						CancelTran()
						return {false, 22222}
					end
				end
			end
		end
	end
	
	--若是含有佣兵团宣战事件需求的任务，判断是否已经完成，已经完成的话直接加计数
	if g_VarQuestMgr["完成佣兵团宣战"] then
		for i=0,#g_VarQuestMgr["完成佣兵团宣战"] do
			if g_VarQuestMgr["完成佣兵团宣战"][i] == sQuestName then
				local GasTongWarDB = RequireDbBox("GasTongWarDB")
				local IsSendChallenge = GasTongWarDB.IsSendChallenge(PlayerId)
				if IsSendChallenge then
					local param = {}
					param["char_id"] = PlayerId
					param["sVarName"] = "完成佣兵团宣战"
					param["iNum"] = 1
					param["CanShareTeamMateTbl"] = {}
					table.insert(QuestVarTbl, AddVarNumForTeamQuests(param))
				end
			end
		end
	end
	
	--若是含有修行塔事件需求的任务，判断是否已经完成，已经完成的话直接加计数
	if g_VarQuestMgr["完成修行塔"] then
		for i=0,#g_VarQuestMgr["完成修行塔"] do
			if g_VarQuestMgr["完成修行塔"][i] == sQuestName then
				local ActivityCountDB = RequireDbBox("ActivityCountDB")
				local Count = ActivityCountDB.GetActivityHistoryTimes(PlayerId, "佣兵训练活动")
				if Count and Count>0 then
					local param = {}
					param["char_id"] = PlayerId
					param["sVarName"] = "完成修行塔"
					param["iNum"] = 1
					param["CanShareTeamMateTbl"] = {}
					table.insert(QuestVarTbl, AddVarNumForTeamQuests(param))
				end
			end
		end
	end
	
	--若是含有守塔事件需求的任务，判断是否已经完成，已经完成的话直接加计数
	if g_VarQuestMgr["完成守塔"] then
		for i=0,#g_VarQuestMgr["完成守塔"] do
			if g_VarQuestMgr["完成守塔"][i] == sQuestName then
				local ActivityCountDB = RequireDbBox("ActivityCountDB")
				local Count = ActivityCountDB.GetActivityHistoryTimes(PlayerId, "异界守护塔队伍")
				if Count and Count>0 then
					local param = {}
					param["char_id"] = PlayerId
					param["sVarName"] = "完成守塔"
					param["iNum"] = 1
					param["CanShareTeamMateTbl"] = {}
					table.insert(QuestVarTbl, AddVarNumForTeamQuests(param))
				end
			end
		end
	end
	
	if g_VarQuestMgr["完成抱小猪选拔赛单人版"] then
		for i=0,#g_VarQuestMgr["完成抱小猪选拔赛单人版"] do
			if g_VarQuestMgr["完成抱小猪选拔赛单人版"][i] == sQuestName then
				local ActivityCountDB = RequireDbBox("ActivityCountDB")
				local Count = ActivityCountDB.GetActivityHistoryTimes(PlayerId, "抱小猪选拔赛单人版")
				if Count and Count>0 then
					local param = {}
					param["char_id"] = PlayerId
					param["sVarName"] = "完成抱小猪选拔赛单人版"
					param["iNum"] = 1
					param["CanShareTeamMateTbl"] = {}
					table.insert(QuestVarTbl, AddVarNumForTeamQuests(param))
				end
			end
		end
	end
	
	if g_VarQuestMgr["完成入团"] then
		for i=0,#g_VarQuestMgr["完成入团"] do
			if g_VarQuestMgr["完成入团"][i] == sQuestName then
				local GasTongBasicDB = RequireDbBox("GasTongBasicDB")
				local TongID = GasTongBasicDB.GetTongID(PlayerId)
				if TongID>0 then
					local param = {}
					param["char_id"] = PlayerId
					param["sVarName"] = "完成入团"
					param["iNum"] = 1
					param["CanShareTeamMateTbl"] = {}
					table.insert(QuestVarTbl, AddVarNumForTeamQuests(param))
				end
			end
		end
	end
	
	if g_VarQuestMgr["完成建团"] then
		for i=0,#g_VarQuestMgr["完成建团"] do
			if g_VarQuestMgr["完成建团"][i] == sQuestName then
				local GasTongBasicDB = RequireDbBox("GasTongBasicDB")
				local InitiatorId = GasTongBasicDB.GetTongInitiator( GasTongBasicDB.GetTongID(PlayerId) )
				if InitiatorId == PlayerId then
					local param = {}
					param["char_id"] = PlayerId
					param["sVarName"] = "完成建团"
					param["iNum"] = 1
					param["CanShareTeamMateTbl"] = {}
					table.insert(QuestVarTbl, AddVarNumForTeamQuests(param))
				end
			end
		end
	end
	
	for _, BuildingName in pairs(TongProdItem_Common:GetKeys()) do
		local VarName = "完成"..BuildingName
		if TongProdItem_Common(BuildingName, "Type") == 1 and g_VarQuestMgr[VarName] then
			for i=0,#g_VarQuestMgr[VarName] do
				if g_VarQuestMgr[VarName][i] == sQuestName then
					local GasTongBasicDB = RequireDbBox("GasTongBasicDB")
					local GasTongbuildingDB = RequireDbBox("GasTongbuildingDB")
					local TongID = GasTongBasicDB.GetTongID(PlayerId)
					local Count = GasTongbuildingDB.CountBuildByName(TongID, BuildingName)
					if Count and Count>0 then
						local param = {}
						param["char_id"] = PlayerId
						param["sVarName"] = VarName
						param["iNum"] = Count
						param["CanShareTeamMateTbl"] = {}
						table.insert(QuestVarTbl, AddVarNumForTeamQuests(param))
					end
				end
			end
		end
	end
	
	local LoopQuestTbl = {"", 0}
	if Quest_Common(sQuestName, "任务类型") == 10 then
		local tbl = GetCfgTransformValue(true, "Quest_Common", sQuestName, "环属性", "1")
		local loopname = tbl[1]
		local loopnum = 0
		local res = StmtContainer._GetQuestLoop:ExecStat(PlayerId, loopname)
		local isfirstquest = false
		if not LoopNum then
			if g_LoopQuestMgr[loopname][1][1].QuestName == sQuestName then
				isfirstquest = true		--为防止某些原因引起的数据库信息异常，若玩家能再次接取第一环，则刷新数据库记录
			end
			if res and res:GetRowNum()>0 and not isfirstquest then
				loopnum = res:GetData(0, 0) + 1
			else
				loopnum = 1
			end
		else
			loopnum = LoopNum
		end
		StmtContainer._UpdateQuestLoop:ExecStat(PlayerId, loopname, loopnum)
		LoopQuestTbl = {loopname, loopnum}
	end
	
--	local QuestNeedVarTbl = {}
--	if Quest_Common(sQuestName, "物品需求") then
--		local Keys = Quest_Common:GetKeys(sQuestName, "物品需求")
--		for k = 1, table.getn(Keys) do
--			local Arg = GetCfgTransformValue(true, "Quest_Common", sQuestName, "物品需求", Keys[k], "Arg")
--			local needtype = Arg[1]
--			local needname = Arg[2]	--需求的物品名
--			if not g_QuestPropMgr[needname] 
--				or g_QuestPropMgr[needname][1] ~= needtype 
--				or g_QuestPropMgr[needname][2] ~= sQuestName then
--				local NeedVarTbl = GetItemVarNum(PlayerId,sQuestName, needtype, needname)
--				table.insert(QuestNeedVarTbl,NeedVarTbl)
--				--CancelTran()
--				--return false,"改变变量数目失败"
--			end
--		end
--	end
--	local IncreQuestItemVarTbl = {}
--	for i=1, table.getn(give_items) do
--		local give_type,give_name,num,res=give_items[i][1],give_items[i][2],give_items[i][3],give_items[i][4]
--		local itemTbl = add_item(PlayerId,give_type, give_name,num, res)
--		table.insert(IncreQuestItemVarTbl,itemTbl)
--	end
	
	--Renton如果接的是一个学习技能的任务，那么在这里面判断已有的技能的等级是否大于等于任务的需求等级
	--局部变量skillLevelNeed 存储结果
	local skillQuestTbl = {}
	if skillNeedQuestInfoTbl then
		local skillname = ""
		local skilllevel = 0
		local nowskilllevel = 0
		local nowskilllevel_value = -1
		local result = false
		local varname = ""
		local skillQuestTbl_i = 0
		local skillLevelMatchNeed = false
		for _,v in pairs(skillNeedQuestInfoTbl) do
			if v.NeedType == "技能需求" then
				local FightSkillDB = RequireDbBox("FightSkillDB") 
				skillname = v.SkillName
				skilllevel = v.SkillLevel
				local datatbl = {}
				datatbl["PlayerId"] = PlayerId
				datatbl["SkillName"] = skillname
				datatbl["fs_kind"] = FightSkillKind.Skill
				result,nowskilllevel_value = FightSkillDB.Lua_SelectFightSkill(datatbl)
				
				--等级的转化，数值等级转化为技能等级
				local now_skill_level = -1
				if not SkillPart_Common(skillname,"PlayerLevelToSkillLevel") then
					LogErr(skillname.."不是正确的技能的真实名或者，SkillPart_Common里面这张表这一技能行有问题")
					CancelTran()
					return {false, 33333}
				else
					local temptbl = loadstring("return "..SkillPart_Common(skillname,"PlayerLevelToSkillLevel"))()
					for i,v in ipairs(temptbl) do
						if v[3] == nowskilllevel_value then
							now_skill_level = i
						end
					end
					skillLevelMatchNeed = false
					if result and (temptbl[1][3]=="l" or now_skill_level >= v.SkillLevel) then
						skillLevelMatchNeed = true
					end
				--加计数
					local temp_varname = _
					local addnum_tbl= {}
					if skillLevelMatchNeed then
						addnum_tbl["iNum"] = 1
						addnum_tbl["char_id"] = PlayerId
						addnum_tbl["sQuestName"] = sQuestName
						addnum_tbl["sVarName"] = temp_varname
						local RoleQuestDB = RequireDbBox("RoleQuestDB")
						local IsTrue = RoleQuestDB.AddQuestVarNum(addnum_tbl)					
							if IsTrue then
							skillQuestTbl_i = skillQuestTbl_i + 1
							skillQuestTbl[skillQuestTbl_i] = {}
							skillQuestTbl[skillQuestTbl_i]["iNum"] = 1
							skillQuestTbl[skillQuestTbl_i]["char_id"] = PlayerId
							skillQuestTbl[skillQuestTbl_i]["sQuestName"] = sQuestName
							skillQuestTbl[skillQuestTbl_i]["sVarName"] = temp_varname
							skillQuestTbl[skillQuestTbl_i].IsTrue = IsTrue
						else
							CancelTran()
							return {false, 44444}
						end
					end
				end
			end
		end
	end
	
	local DirectDB = RequireDbBox("DirectDB")
	local DirectTbl = DirectDB.AddPlayerQuestDirect(PlayerId, "接取任务", sQuestName)
	return {true,give_items,YbItemIndex,QuestVarTbl,DirectTbl,skillQuestTbl,LoopQuestTbl}
end

--添加任务
function RoleQuestDB.AddQuest(data)
	return AddQuest(data)
end

--GM指令添加任务物品
--function RoleQuestDB.GMAddQuestVar(data)
--	local lGMDBExecutor = RequireDbBox("GMDB")
--	local result = lGMDBExecutor.AddItem(data)
--	if result[1] then
--		local itemTbl = IncreQuestItemVar(data["player_id"],data["itemtype"], data["itemname"], data["count"])
--		return {result[1],result[2],itemTbl}
--	else
--		return result
--	end
--end

--添加任务ITEM,并且返回添加的ITEM信息
--function RoleQuestDB.AddQuestItemVar(data)
--	if IsNumber(data["itemTbl"]) then
--		return {}
--	else
--		return add_item(data["char_id"],data["nType"], data["sName"], data["nCount"],data["itemTbl"])
--	end
--end

local function GMAddQuest(data)
	local QuestName = data["QuestName"]
	if not Quest_Common(QuestName) then
		return {false,3083} --"没有此任务"
	end
	local PlayerId = data["PlayerId"]
	local state = RoleQuestDB.GetQuestState({PlayerId, QuestName})
	if state ~= nil then
		if state == QuestState.init then
			if not RoleQuestDB.DeleteAllQuestVar(QuestName, PlayerId) then
				CancelTran()
				return {false,3084} --"清除任务记数失败"
			end
		end
		if not RoleQuestDB.ClearQuest(QuestName,PlayerId) then
			CancelTran()
			return {false,3085} --"清除任务失败"
		end
	end
	
	local RoleQuestTbl = RoleQuestDB.AddQuest(data)
	if RoleQuestTbl[1] then
		return {true,state,RoleQuestTbl[2],RoleQuestTbl[3],RoleQuestTbl[4],RoleQuestTbl[5],RoleQuestTbl[6],RoleQuestTbl[7]}
	else
		return RoleQuestTbl
	end
end

--GM指令添加任务
function RoleQuestDB.GMAddQuest(data)
	return GMAddQuest(data)
end

--GM指令完成任务
local function GMFinishQuest(data)
	local QuestName = data["QuestName"]
	local sceneName = data["sceneName"]
	if not Quest_Common(QuestName) then
		return {false,3086} --"任务不存在"
	end
	
	local PlayerId = data["PlayerId"]
	local state = RoleQuestDB.GetQuestState({PlayerId, QuestName})
	if state == nil then
		return {false,3087} --"没有接取该任务"
	end
	if state == QuestState.finish then
		return {false,3088} --"任务已完成"
	end
	
	local del_items={}
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	
	if Quest_Common(QuestName, "物品需求") then
		local Keys = Quest_Common:GetKeys(QuestName, "物品需求")
		for k = 1, table.getn(Keys) do
			local Arg = GetCfgTransformValue(true, "Quest_Common", QuestName, "物品需求", Keys[k], "Arg")
			local needtype = Arg[1]
			local needname = Arg[2]		--需求的物品名
			local neednum = Arg[3]
			local have_num = g_RoomMgr.GetItemCount(PlayerId,needtype,needname)
			if needtype ~= 16 and have_num>=neednum then
				local res = g_RoomMgr.DelItem(PlayerId, needtype,needname,neednum,nil,event_type_tbl["任务品或者道具物品删除"])
				if IsNumber(res) then
					CancelTran()
					return {false,3089} --"需求物品删除失败"
				end
				table.insert(del_items,{needtype,needname,neednum,res})
			elseif have_num>0 then
				local res = g_RoomMgr.DelItem(PlayerId, needtype,needname,have_num,nil,event_type_tbl["任务品或者道具物品删除"])
				if IsNumber(res) then
					CancelTran()
					return {false,3089} --"需求物品删除失败"
				end
				table.insert(del_items,{needtype,needname,have_num,res})
			end
		end
	end
	
	--清空任务变量
	if not RoleQuestDB.DeleteAllQuestVar(QuestName,PlayerId) then
		CancelTran()
		return {false,3090} --"清空任务变量失败"
	end
	--设置任务状态
	if not RoleQuestDB.UpdateQuestState(QuestState.finish,PlayerId,QuestName) then
		CancelTran()
		return {false,300005} --"设置任务状态失败"
	end
	
	if not RoleQuestDB.UpdateCharFinishQuestNum(PlayerId,1) then
		CancelTran()
		return {false,3091} --"增加任务完成数量失败"
	end
	
	--清空任务道具
	if Quest_Common(QuestName, "获得物品") then
		local Keys = Quest_Common:GetKeys(QuestName, "获得物品")
		for i = 1,table.getn(Keys) do
			local tbl = GetCfgTransformValue(true, "Quest_Common", QuestName, "获得物品", Keys[i])
			if tbl[3] then
				local give_type = tbl[1]
				local give_name = tbl[2]
				local del_num = g_RoomMgr.GetItemCount(PlayerId,give_type,give_name)
				if(del_num>0) then
					local res = g_RoomMgr.DelItem(PlayerId, give_type, give_name,del_num,nil,event_type_tbl["任务品或者道具物品删除"])
					if IsNumber(res) then
						CancelTran()
						return {false,res}
					end
					table.insert(del_items,{give_type,give_name,del_num, res})
				end
			end
		end
	end
	
	local LoopQuestTbl = {"", 0}
	local loopname, loopnum = "", 0
	if Quest_Common(QuestName, "任务类型") == 10 then
		local tbl = GetCfgTransformValue(true, "Quest_Common", QuestName, "环属性", "1")
		local res = StmtContainer._GetQuestLoop:ExecStat(PlayerId, tbl[1])
		if res and res:GetRowNum()>0 then
			local PlayerLevel = data["PlayerLevel"]
			loopname = tbl[1]
			loopnum = res:GetData(0, 0)
			local loopmgr = g_LoopQuestMgr[loopname][loopnum+1]
			local n = 0
			local temptbl = {}
			if loopmgr then
				for i=1, table.getn(loopmgr) do
					if sQuestName ~= loopmgr[i].QuestName
						and PlayerLevel >= loopmgr[i].MinLevel and PlayerLevel <= loopmgr[i].MaxLevel then
						table.insert(temptbl, loopmgr[i].QuestName)
					end
				end
				n = table.getn(temptbl)
				if n > 0 then
					LoopQuestTbl[1] = temptbl[math.random(1, n)]
					LoopQuestTbl[2] = loopnum+1
				end
			end
			if n <= 0 then
				StmtContainer._DeleteQuestLoop:ExecStat(PlayerId, loopname)
				LoopQuestTbl[1] = "LoopOver"
				LoopQuestTbl[2] = 0
			end
		end
	end
	
	local uExpMultiple = RoleQuestDB.GetQuestExpMultiple(PlayerId,QuestName)
	local succ,add_items,AddExpTbl,SoulRet,MercenaryLevelTbl,uAddSoul = RoleQuestDB.GiveQuestAward(data,uExpMultiple,sceneName,loopnum)
	if not succ then
		return {false,add_items}
	end
	--去掉刷区域功能，用区域副本形式体现
--	local AreaDB = RequireDbBox("AreaDB")
--	local IsFinish, AreaName = AreaDB.FinishAreaQuest(PlayerId, QuestName)
--	if not IsFinish then
--		retun {false,nil}
--	end
	
	local delItemTbl = {}
	for i=1,table.getn(del_items) do
		local type,name,num,res=del_items[i][1],del_items[i][2],del_items[i][3],del_items[i][4]
		local Tbl = {}
		Tbl["nType"] = type
		Tbl["sName"] = name
		Tbl["nCount"] = num
		Tbl["res"] = res
		table.insert(delItemTbl, Tbl)
	end
	
	local DirectDB = RequireDbBox("DirectDB")
	local DirectTbl = DirectDB.AddPlayerQuestDirect(PlayerId, "完成任务", QuestName)
	
	local addItemTbl = {}
--	local param = {}
--	param["char_id"] = PlayerId
	for i=1, table.getn(add_items) do
--		param["nType"] = add_items[i][1]
--		param["sName"] = add_items[i][2]
--		param["nCount"] = add_items[i][3]
--		param["itemTbl"] = add_items[i][4]
		local Tbl = {}
		--Tbl["QuestItemVar"] = RoleQuestDB.AddQuestItemVar(param)
		Tbl["nType"] = add_items[i][1]
		Tbl["sName"] = add_items[i][2]
		Tbl["nCount"] = add_items[i][3]
		Tbl["res"] = add_items[i][4]
		Tbl["msgType"] = add_items[i][5]
		DirectDB.AddPlayerItemDirect(PlayerId, "任务获得物品", Tbl["nType"], Tbl["sName"], DirectTbl)
		table.insert(addItemTbl, Tbl)
	end
	
	local AddLoopQuestResult = nil
	if LoopQuestTbl[1] ~= "" and LoopQuestTbl[1] ~= "LoopOver" then
		local param = {}
		param["PlayerId"] = PlayerId
		param["QuestName"] = LoopQuestTbl[1]
		param["sceneName"] = sceneName
		param["LoopNum"] = LoopQuestTbl[2]
		AddLoopQuestResult = AddQuest(param)
	end
	
	return {true,delItemTbl,addItemTbl,AddExpTbl,SoulRet,MercenaryLevelTbl,DirectTbl,LoopQuestTbl,uAddSoul,AddLoopQuestResult}
end

function RoleQuestDB.GMFinishQuest(data)
	return GMFinishQuest(data)
end

local function CanAcceptQuest(data)
	local PlayerId = data["PlayerId"]
	local JoinActionDB = RequireDbBox("JoinActionDB")
	if not JoinActionDB.CheckWarnValue(PlayerId) then
		return false
	end
	
	local sQuestName = data["QuestName"]
	local QuestNode = Quest_Common(sQuestName)
	local state = RoleQuestDB.GetQuestState({PlayerId,sQuestName})
	if state == QuestState.init then
		return false,3063 --"你身上已经有此任务"
	end
	if(state~=nil and state==QuestState.finish and 
		not g_RepeatQuestMgr[sQuestName]) then
		--判断是否已经完成该任务，是否是可重复任务
		return false,3064 --"不是可重复任务"
	end
	
	--判断接任务个数是不是超过10个
	local QuestNumRes = StmtContainer._GetDoingQuestNum:ExecSql("s[32]", PlayerId)
	if(QuestNumRes:GetRowNum() >= 10) then
		local iQuestNum = {0,0,0,0}
		for i=1, QuestNumRes:GetRowNum() do
			local QuestName = QuestNumRes:GetData(i-1, 0)
			if g_AllMercenaryQuestMgr[QuestName] then--是佣兵任务
				iQuestNum[1] = iQuestNum[1] + 1
			elseif g_DareQuestMgr[QuestName] then
				iQuestNum[2] = iQuestNum[2] + 1
			elseif g_ActionQuestMgr[QuestName] then
				iQuestNum[3] = iQuestNum[3] + 1
			else
				iQuestNum[4] = iQuestNum[4] + 1
			end
		end
		QuestNumRes:Release()
		local CheckNum = 4
		if g_AllMercenaryQuestMgr[sQuestName] then--是佣兵任务
			CheckNum = 1
		elseif g_DareQuestMgr[sQuestName] then
			CheckNum = 2
		elseif g_ActionQuestMgr[sQuestName] then
			CheckNum = 3
		end
		if iQuestNum[CheckNum] >= 10 then
			return false,3065 --"任务数量已经到达上限,你最多可以同时接10个任务"
		end
	end
	
	
	--检测这个任务,是不是可以接
	if sQuestName == "莱斯林克的祝福" then
		local param = {}
		param["PlayerId"] = PlayerId
		param["ActivityName"] = sQuestName
		local ActivityCountDbBox = RequireDbBox("ActivityCountDB")
		if not ActivityCountDbBox.CheckSinglePlayerActivityCount(param) then
			return false,3066 --"你在这一周,已经接了一次委托了,下周再挑战吧,佣兵"
		end
	end
	
	--判断前置任务(已完成,进行中)
	if QuestNode("前置任务") then
		local Keys = QuestNode:GetKeys("前置任务")
		local IsAvailable = true
		local IsEffective = false
		for i = 1,table.getn(Keys) do
			local Subject = QuestNode("前置任务", Keys[i], "Subject")
			local Arg = QuestNode("前置任务", Keys[i], "Arg")
			if Subject ~= "" then
				IsEffective = true
				local pre_state = RoleQuestDB.GetQuestState{PlayerId, Subject}
				if Arg == "已完成" and pre_state ~= QuestState.finish then
					IsAvailable = false
				elseif Arg == "进行中" and pre_state ~= QuestState.finish and pre_state ~= QuestState.init then
					IsAvailable = false
				end
			end
		end
		if IsEffective and not IsAvailable then
			return false,3068 --"前提任务尚未完成"
		end
	end
	
	--判断前置任务(任意一个任务状态满足,就行)
	if QuestNode("任意前置满足其一") then
		local Keys = QuestNode:GetKeys("任意前置满足其一")
		local IsAvailable = false
		local IsEffective = false
		for i = 1,table.getn(Keys) do
			local Subject = QuestNode("任意前置满足其一", Keys[i], "Subject")
			local Arg = QuestNode("任意前置满足其一", Keys[i], "Arg")
			if Subject ~= "" then
				IsEffective = true
				local pre_state = RoleQuestDB.GetQuestState{PlayerId, Subject}
				if Arg == "已完成" and pre_state == QuestState.finish then
					IsAvailable = true
					break
				elseif Arg == "进行中" and pre_state ~= QuestState.failure then
					IsAvailable = true
					break
				end
			end
		end
		if IsEffective and not IsAvailable then
			return false,3068 --"前提任务尚未完成"
		end
	end
	
	local PlayerCamp = data["PlayerCamp"]
	local PlayerLev = data["PlayerLevel"]
	local PlayerYbLev = data["PlayerYbLevel"]
	
	if not IfAcceptQuestCamp(PlayerCamp, sQuestName) then     --阵营判断
		return false,3069 --"你的阵营不符合"
	end
	
	if not IfAcceptQuestMinLev(PlayerLev, sQuestName) then    --等级判断
		return false,3070 --"你的等级不够"
	end
	
	if not IfAcceptQuestMaxLev(PlayerLev, sQuestName) then    --等级判断
		return false,300023 --"你的等级过高"
	end
	
	if g_AllMercenaryQuestMgr[sQuestName] then
		if not CmpMercenaryLevel(PlayerYbLev, sQuestName) then
			return false,3071 --"你的佣兵等级不够"
		end
	end
	
	local TongLevLimit = QuestNode("佣兵团限制")
	if TongLevLimit and TongLevLimit ~= 0 then
		local GasTongBasicDB = RequireDbBox("GasTongBasicDB")
		local TongLev = GasTongBasicDB.GetTongLevel(GasTongBasicDB.GetTongID(PlayerId))
		if TongLev < TongLevLimit then
			return false, 300024, {TongLevLimit}
		end
	end
	
	--时间限次检测
	local GMTimeCountLimit = data["GMTimeCountLimit"]
	local TimeCountNode = QuestNode("时间限次")
	if TimeCountNode and GMTimeCountLimit == 1 then
		local TimeTbl = GetCfgTransformValue(true, "Quest_Common", sQuestName, "时间限次", "Subject")
		local Cycle = TimeTbl[3]
		local Count = TimeCountNode("Arg")
		local GasTongBasicDB = RequireDbBox("GasTongBasicDB")
		local TongId = GasTongBasicDB.GetTongID(PlayerId)
		local TongType = GasTongBasicDB.GetTongType(TongId)
		if TongId~=0 and (sQuestName == "艾米团委托·起始任务" or sQuestName == "神圣团委托·起始任务" or sQuestName == "修斯团委托·起始任务")
			and (TongType == g_TongMgr:GetTongTypeByName("生产") or TongType == g_TongMgr:GetTongTypeByName("精英生产")) then
			Count = Count + 1
		end
		local secc1, secc2 = CheckQuestTimeCount(sQuestName, PlayerId, TimeTbl, Count)
		if not secc1 then
			if secc2 then
				if secc1 == false then
					return false, 300006 --"没到接取时间，现在还不能接取该任务"
				else
					return false, 300025 --"没到接取时间，现在还不能接取该任务"
				end
			else
				if Cycle >= 1 then
					return false, 300007, {Cycle, Count} --"Cycle.."小时之内你只能完成"..Count.."次该任务"
				else
					return false, 300019, {Cycle*60, Count}
				end
			end
		end
	end
	
	local mutexQuestTbl = GetCfgTransformValue(true, "Quest_Common", sQuestName, "互斥关系")
	if mutexQuestTbl then
		if QuestNode("时间限次") then
			for _, name in pairs(mutexQuestTbl) do
				if name ~= sQuestName then
					local q_state = RoleQuestDB.GetQuestState{PlayerId, name}
					if q_state and q_state ~= QuestState.finish then 
						return false, 300027
					end
				end
			end
			local TimeTbl = GetCfgTransformValue(true, "Quest_Common", sQuestName, "时间限次", "Subject")
			local Cycle = TimeTbl[3]
			local Count = TimeCountNode("Arg")
			for _, name in pairs(mutexQuestTbl) do
				if name ~= sQuestName then
					local secc1, secc2 = CheckQuestTimeCount(name, PlayerId, TimeTbl, Count)
					if not secc1 then
						if Cycle >= 1 then
							return false, 300028, {Cycle, Count} --"Cycle.."小时之内你只能完成"..Count.."次该任务"
						else
							return false, 300029, {Cycle*60, Count}
						end
					end
				end
			end
		else
			for _, name in pairs(mutexQuestTbl) do
				local q_state = RoleQuestDB.GetQuestState{PlayerId, name}
				if q_state ~= nil then 
					return false,3067 --"任务互斥"
				end
			end
		end
	end
	
	--冷却时间检测
	local CDTime = QuestNode("冷却时间")
	if CDTime and CDTime~=0 then
		local secc, str = CheckQuestCD(sQuestName, PlayerId, CDTime)
		if not secc then
			return false, 300008, {CDTime, str} --"该任务"..CDTime.."小时内只能接取一次, 请在"..str.."后再来"
		end
	end
	
	--任务接取时间检测
	if QuestNode("任务接取时间") then
		local secc = CheckStartTime(sQuestName)
		if not secc then
			return false, 300013	--"还没到该任务的接取时间, 请过一会儿再来"
		end
	end
	
	return true
end

--判断是否可以接取任务
function RoleQuestDB.CanAcceptQuest(data)
	return CanAcceptQuest(data)
end

local function AcceptQuestNumLimit(PlayerId, TypeTbl, ActivityName)
	local mercParam = {}
	mercParam["PlayerId"] = PlayerId
	mercParam["ActivityName"] = ActivityName
	local ActivityCountDbBox = RequireDbBox("ActivityCountDB")
	local IsAllow, FinishTimes = ActivityCountDbBox.CheckSinglePlayerActivityCount(mercParam)
	if not IsAllow then
		return false , FinishTimes
	else
		--print("1.......", FinishTimes)
		--判断接任务个数是不是超过10个
		local QuestNumRes = StmtContainer._GetDoingQuestNum:ExecSql("s[32]", PlayerId)
		if QuestNumRes:GetRowNum() > 0 then
			for i=1, QuestNumRes:GetRowNum() do
				local sQuestName = QuestNumRes:GetData(i-1,0)
				if g_AllMercenaryQuestMgr[sQuestName] then
					local MercenaryType = Quest_Common(sQuestName, "佣兵任务类型")
					--如果是佣兵日常任务,检查任务是否已经完成了10个
					if TypeTbl[MercenaryType] then
						FinishTimes = FinishTimes + 1
					end
				end
			end
			QuestNumRes:Release()
		end
		--print("2.......", FinishTimes)
		if FinishTimes >= ActivityCountDbBox.GetActivityCount(ActivityName) then
			return false, FinishTimes
		end
	end
	return true
end

local function AcceptQuest(data)
	local PlayerId = data["PlayerId"]
	local QuestName = data["QuestName"]
	--先检查一下接了几个佣兵任务,并且今天完成了几次了
	if g_AllMercenaryQuestMgr[QuestName] then
		local MercenaryType = Quest_Common(QuestName, "佣兵任务类型")
		--如果是佣兵日常任务或者是小队任务,检查任务是否已经完成了10个
		local ActivityName = nil
		if MercenaryType == 1 then
			local res1, msgInfo = AcceptQuestNumLimit(PlayerId, {[1]=true}, "佣兵日常任务")
			if not res1 then
				return {false, 300009, {msgInfo}}--"今天你接取或完成的佣兵日常任务的个数已经够10个，不能再接取佣兵日常日任务"
			end
		elseif MercenaryType == 6 then
			local res,DBTime = GetQuestStateFinishTime(PlayerId,QuestName)
			if res and res == QuestState.finish then
				local nowDate = os.date("*t",os.time())
				local dbDate = os.date("*t",DBTime)
				if (nowDate.year == dbDate.year)
					and (nowDate.month == dbDate.month)
					and (nowDate.day == dbDate.day) then
					return {false, 300020}--如果是同一天的话,就不让接这个任务了,同一个任务,1天只能做一次
				end
			end
		elseif MercenaryType == 7 then
			local res1, msgInfo = AcceptQuestNumLimit(PlayerId, {[7]=true}, "佣兵小队任务")
			if not res1 then
				return {false, 300014, {msgInfo}}--"今天你接取或完成的佣兵日常任务的个数已经够10个，不能再接取佣兵日常日任务"
			end
		end
	end
	
	local succ,id,arg = RoleQuestDB.CanAcceptQuest(data)
	if not succ then
		return {false,id,arg}
	else
		local result = RoleQuestDB.AddQuest(data)
		if result[1] then
			if data["QuestName"] == "莱斯林克的祝福" then
				local param = {}
				param["PlayerId"] = data["PlayerId"]
				param["ActivityName"] = data["QuestName"]
				local ActivityCountDbBox = RequireDbBox("ActivityCountDB")
				ActivityCountDbBox.AddActivityCount(param)
			end
		end
		return result
	end
end

--接一个新的任务
function RoleQuestDB.AcceptQuest(data)
	return AcceptQuest(data)
end

local function ItemRequestQuest(data)
	local sQuestName = data["QuestName"]
	local PlayerId = data["PlayerId"]
	local sceneName = data["sceneName"]
	local ItemName = data["ItemName"]
	local ItemType = 1
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local del_num = g_RoomMgr.GetItemCount(PlayerId, 1, ItemName)
	if del_num < 1 then
		ItemType = 16
		del_num = g_RoomMgr.GetItemCount(PlayerId, 16, ItemName)
		if del_num < 1 then
			return {false, 3072} --"触发任务的物品不符合要求"
		end
	end
	
	--清空任务物品
	local del_items = {}
	local res = g_RoomMgr.DelItem(PlayerId, ItemType,ItemName,1,nil,event_type_tbl["任务品或者道具物品删除"])
	if IsNumber(res) then
		CancelTran()
		return {false, 3073} --"清空任务物品失败"
	end
	table.insert(del_items,{ItemType,ItemName,1,res})

	local succ,id,arg = RoleQuestDB.CanAcceptQuest(data)
	if not succ then
		CancelTran()
		return {false,id,arg}
	else
		return RoleQuestDB.AddQuest(data),del_items
	end
end

--物品触发任务
function RoleQuestDB.ItemRequestQuest(data)
	local PlayerId = data["PlayerId"]
	local PlayerQuestInfo,DelItem = ItemRequestQuest(data)
	
	local TeamMemberQuestInfo = {}
	if PlayerQuestInfo[1] and data["IsYbQuestItem"] then
		local GasTeamDB =  RequireDbBox("GasTeamDB")
		local TeamID = GasTeamDB.GetTeamID(PlayerId)
		if TeamID and TeamID ~= 0 then
			local tblMembers = GasTeamDB.GetTeamMembers(TeamID)
			
			local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
			
			local param = {}
			param["QuestName"] = data["QuestName"]
			param["PlayerCamp"] = data["PlayerCamp"]
			param["sceneName"] = data["sceneName"]
			for i=1, #(tblMembers) do
				local id = tblMembers[i][1]--得到角色的ID
				if PlayerId ~= id then
					param["PlayerId"] = id
					local Lev_Exp = CharacterMediatorDB.GetCharLevelExpDB({["char_id"] = id})
					local DBLevel = Lev_Exp:GetData(0,0)
					local DBYbLevel = Lev_Exp:GetData(0,2)
					param["PlayerLevel"] = DBLevel
					param["PlayerYbLevel"] = DBYbLevel
					local succ,id,arg = RoleQuestDB.CanAcceptQuest(param)
					if not succ then
						CancelTran()
						return {false,3074} --"接任务失败,队伍中,有人接任务的条件不满足"
					end
				end
			end
		end
	end
	
	return PlayerQuestInfo,DelItem,TeamMemberQuestInfo
end

local function ShareQuest(data)
	local PlayerId = data["PlayerId"]
	local QuestName = data["QuestName"]
	local TeamId = data["TeamID"]
	local team = RequireDbBox("GasTeamDB")
	local tblMembers = team.GetTeamMembers(TeamId)
	if table.getn(tblMembers) == 0 then
		return {false,3007}
	end
	if RoleQuestDB.GetQuestState({PlayerId,QuestName}) ~= QuestState.init then
		return {false,3006}
	end
	local LoginServerSql = RequireDbBox("LoginServerDB")
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local TeamQuestInfoTbl = {}
	for i=1, table.getn(tblMembers) do
		local teammateid = tblMembers[i][1]
		data["PlayerId"] = teammateid
		if PlayerId ~= teammateid and LoginServerSql.IsPlayerOnLine(teammateid) then
			data["char_id"] = teammateid
			local Lev_Exp = CharacterMediatorDB.GetCharLevelExpDB(data)
			local DBLevel = Lev_Exp:GetData(0,0)
			local DBYbLevel = Lev_Exp:GetData(0,2)
			Lev_Exp:Release()
			data["PlayerLevel"] = DBLevel
			data["PlayerYbLevel"] = DBYbLevel
 			local succ,id,arg = RoleQuestDB.CanAcceptQuest(data)
			local teamQuestTbl = {}
			teamQuestTbl["PlayerId"] = teammateid
			if succ then
				teamQuestTbl["IsCanShare"] = true
				teamQuestTbl["QuestName"] = QuestName
			else
				teamQuestTbl["IsCanShare"] = false
				teamQuestTbl["MsgID"] = id
				teamQuestTbl["Arg"] = arg or {}
			end
			table.insert(TeamQuestInfoTbl,teamQuestTbl)
 		end
 	end
 	return {true,TeamQuestInfoTbl}
end

--任务共享
function RoleQuestDB.ShareQuest(data)
	return ShareQuest(data)
end

local function CheckNeedItemNum(PlayerId, ItemType, ItemName, CheckQuestNeed)
	local questmgr = g_WhereGiveQuestMgr["Goods"][ItemName]
	if questmgr and ItemType == 16 then
		local questname = questmgr[1]
		local queststate = RoleQuestDB.GetQuestState({PlayerId,questname})
		if queststate == nil or (queststate == QuestState.finish and g_RepeatQuestMgr[questname]) then
			return {true,1}
		end
	end
	if CheckQuestNeed and g_ItemQuestMgr[ItemName] then
		for i = 1, table.getn(g_ItemQuestMgr[ItemName]) do
			if ItemType == g_ItemQuestMgr[ItemName][i][1] then
				local parm = 
				{
					["sQuestName"] = g_ItemQuestMgr[ItemName][i][2],
					["iItemType"] = ItemType,
					["sItemName"] = ItemName,
					["uCharId"] = PlayerId
				}
				local NeedNum = RoleQuestDB.CheckQuestItemNeedNum(parm)				
				if NeedNum then
					return {true,NeedNum}
				end
			end
		end
		return {false,5}
	else
		local Only = g_ItemInfoMgr:GetItemInfo( ItemType, ItemName,"Only" )
		if Only == 1 then
			local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
			local havenum = g_RoomMgr.GetItemCount(PlayerId, ItemType, ItemName)
			if havenum >= 1 then
				return {false,4}
			else
				return {true,1}
			end
		end
		return {true,0}  --0代表需求数量无限制
	end
end

--检测所需的物品是否已经满足，返回需求的物品数量
function RoleQuestDB.CheckNeedItemNum(PlayerId, ItemType, ItemName, CheckQuestNeed)
	return CheckNeedItemNum(PlayerId, ItemType, ItemName, CheckQuestNeed)
end

local function IntoTrapAddItem(data)
	local sQuestName = data["sQuestName"]
	local PlayerId = data["char_id"]
	if sQuestName and sQuestName ~= "" then
		if RoleQuestDB.GetQuestState({PlayerId,sQuestName}) ~= QuestState.init then
			return 5,nil
		end
	end
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	data["createType"] = event_type_tbl["踩trap给物品"]
	return CharacterMediatorDB.AddItem(data)
end

--踩Trap,给玩家加物品
function RoleQuestDB.IntoTrapAddItem(data)
	return IntoTrapAddItem(data)
end

local function SingleGridItem(data,IsTran)
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local res1= CharacterMediatorDB.AddItem(data)
	if IsNumber(res1) then
		if not IsTran then
			CancelTran()
		end
		local RoomMgr = RequireDbBox("GasRoomMgrDB")
		return {false,res1}
	end
	return {true,res1}
end

--添加物品,方法1
function RoleQuestDB.SingleGridItem(data,IsTran)
	return SingleGridItem(data,IsTran)
end

--完成第2个日常佣兵任务,给宝石(10-19)
local function RandomGetStone()
	local tbl = {"粗糙的火灵石","粗糙的风歌石","粗糙的雷鸣石","粗糙的冰晶石","粗糙的黑魔石"}
	local num = math.random(1,#(tbl))
	return tbl[num]
end

--完成第2个日常佣兵任务,给随机给物品(20级后)
local function RandomGetMercItem(DBPlayerYbLevel, PlayerCamp)
	if not PlayerCamp then
		--如果玩家的阵营为NIL,说明1,是用GM完成的任务,不给物品;2,就是有bug(除了GM,不应该会NIL)
		return
	end
	local ItemTbl = {}--目前分2类,1表示基准任务;2表示稀有任务
	for _,ItemName in pairs(Mercenary_Quest_Server:GetKeys()) do
		local Tbl = Mercenary_Quest_Server(ItemName)
		local Camp = Tbl("Camp")
		local Type = Tbl("Type")
		if (Camp == 0 or Camp == PlayerCamp) and (Type == 1 or Type == 2) then--要给一个阵营的物品
			if not ItemTbl[Type] then
				ItemTbl[Type] = {}
			end
			table.insert(ItemTbl[Type],ItemName)
		end
	end
	
	if not ItemTbl[1] then
		return
	end
	
	
	local LenTbl = {0,0}
	
	for i,_ in pairs(ItemTbl) do
		if ItemTbl[i] then
			LenTbl[i] = #(ItemTbl[i])
		end
	end
	
	local Coef = 0
	if DBPlayerYbLevel >= 3 then
		Coef = DBPlayerYbLevel - 2
	end
	
	local MaxRandomNum = LenTbl[1]
	if ItemTbl[2] then
		MaxRandomNum = MaxRandomNum + LenTbl[2]*Coef
	end
	if MaxRandomNum == 0 then
		return
	end
	
	local num = math.random(1,MaxRandomNum)
	if num <= LenTbl[1] then
		return ItemTbl[1][num]
	else
		local iNum = math.random(1,LenTbl[2])
		return ItemTbl[2][iNum]
	end
end

--完成第2个日常佣兵任务,随机给物品(领域传送的物品)(55级后)
local function RandomGetScopesItem(PlayerCamp)
	if not PlayerCamp then
		--如果玩家的阵营为NIL,说明1,是用GM完成的任务,不给物品;2,就是有bug(除了GM,不应该会NIL)
		return
	end
	local ItemTbl = {}--目前分2类,1表示基准任务;2表示稀有任务
	for ItemName,Tbl in pairs(Mercenary_Quest_Server) do
		local Camp = Tbl.Camp
		local Type = Tbl.Type
		if (Camp == 0 or Camp == PlayerCamp) and (Type == 3 or Type == 4) then--要给一个阵营的物品
			if not ItemTbl[Type-2] then
				ItemTbl[Type-2] = {}
			end
			table.insert(ItemTbl[Type-2],ItemName)
		end
	end
	
	local LenTbl = {0,0}
	
	for i,_ in pairs(ItemTbl) do
		if ItemTbl[i] then
			LenTbl[i] = #(ItemTbl[i])
		end
	end
	
	local num = math.random(1,100)
	if num < 90 then
		if LenTbl[1] > 0 then
			local iNum = math.random(1,LenTbl[1])
			return ItemTbl[1][iNum]
		end
	else
		if LenTbl[2] > 0 then
			local iNum = math.random(1,LenTbl[2])
			return ItemTbl[2][iNum]
		end
	end
end

local vipBuffTbl = {
		["G级佣兵福利"] = true,
		["F级佣兵福利"] = true,
		["E级佣兵福利"] = true,
		["D级佣兵福利"] = true,
}

local function GiveQuestAward(data,uExpMultiple,sceneName,LoopNum)
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local PlayerId = data["PlayerId"]
	local sQuestName = data["QuestName"]
	local QuestNode = Quest_Common(sQuestName)
	local SelectIndex = data["SelectIndex"]
	local TongId = data["PlayerTongID"]
	--local PlayerLevel = data["PlayerLevel"]
	data["char_id"] 		= PlayerId
	
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local GasTongBasicDB = RequireDbBox("GasTongBasicDB")
	local MoneyManagerDB = RequireDbBox("MoneyMgrDB")
	local AreaFbPointDB = RequireDbBox("AreaFbPointDB")
	local LogMgr = RequireDbBox("LogMgrDB")
	local EquipMgrDB = RequireDbBox("EquipMgrDB")
	
	local Lev_Exp = CharacterMediatorDB.GetCharLevelExpDB(data)
	local DBLevel = Lev_Exp:GetData(0,0)
	local DBExp = Lev_Exp:GetData(0,1)
	local DBPlayerYbLevel = Lev_Exp:GetData(0,2)
	local DBIntegral = Lev_Exp:GetData(0,3)
	Lev_Exp:Release()
	
	local uEventId = LogMgr.FinishQuest(PlayerId, sQuestName)
	local res_tbl={}
	
	if g_DareMercenaryQuestMgr[sQuestName] then
		--佣兵挑战任务物品奖励必给的奖励
		if Quest_Common(sQuestName, "物品奖励") then
			local Must_give = GetCfgTransformValue(true, "Quest_Common", sQuestName, "物品奖励", "Subject")
			local ItemIndex = GetAwardItemFromQuest(PlayerId, sQuestName)
			
			if ItemIndex and Must_give[ItemIndex] then
				local itemtype,name,num, BindingStyle=Must_give[ItemIndex][1],Must_give[ItemIndex][2],Must_give[ItemIndex][3],Must_give[ItemIndex][4]
				if g_ItemInfoMgr:IsSoulPearl(itemtype) then
					name = CountReturnSoulPearl(DBLevel,PlayerYbLevel,LoopNum,name)
				end
				local params= {}
				params.nCharID = PlayerId
				params.nStoreMain = g_StoreRoomIndex.PlayerBag
				params.nBigID = itemtype
				params.nIndex = name
				params.nCount = num
				params.BindingType = BindingStyle
				params.uEventId = uEventId
				local res = g_RoomMgr.AddItem(params)
				if IsNumber(res) then
					CancelTran()
					return false,res
				end
				table.insert(res_tbl,{itemtype,name,num,res})
			end
		end
		
	else
		if Quest_Common(sQuestName, "物品奖励") then
			--物品奖励必给的奖励
			local Must_give = GetCfgTransformValue(true, "Quest_Common", sQuestName, "物品奖励", "Subject")
			for i=1,table.getn(Must_give) do
				local itemtype,name,num, BindingStyle=Must_give[i][1],Must_give[i][2],Must_give[i][3],Must_give[i][4]
				if g_ItemInfoMgr:IsSoulPearl(itemtype) then
					name = CountReturnSoulPearl(DBLevel,PlayerYbLevel,LoopNum,name)
				end
				local params= {}
				params.nCharID = PlayerId
				params.nStoreMain = g_StoreRoomIndex.PlayerBag
				params.nBigID = itemtype
				params.nIndex = name
				params.nCount = num
				params.BindingType = BindingStyle
				params.uEventId = uEventId
				local res = g_RoomMgr.AddItem(params)
				if IsNumber(res) then
					CancelTran()
					return false,res
				end
				table.insert(res_tbl,{itemtype,name,num,res})
			end
			--物品奖励可选的奖励
			if SelectIndex~=nil and SelectIndex~=0 then
				local select_award = GetCfgTransformValue(true, "Quest_Common", sQuestName, "物品奖励", "Arg")
				if (select_award == nil or not select_award[SelectIndex]) then
					CancelTran()
					return false, 3093 --"不存在可选物品奖励！"
				end
				local itemtype,name,num, BindingStyle = select_award[SelectIndex][1], select_award[SelectIndex][2], select_award[SelectIndex][3], select_award[SelectIndex][4]
				if g_ItemInfoMgr:IsSoulPearl(itemtype) then
					name = CountReturnSoulPearl(DBLevel,PlayerYbLevel,LoopNum,name)
				end
				local params= {}
				params.nCharID = PlayerId
				params.nStoreMain = g_StoreRoomIndex.PlayerBag
				params.nBigID = itemtype
				params.nIndex = name
				params.nCount = num
				params.BindingType = BindingStyle
				params.uEventId = uEventId
				local res = g_RoomMgr.AddItem(params)
				if IsNumber(res) then
					CancelTran()
					return false,res
				end
				table.insert(res_tbl, {itemtype,name,num,res})
			end
		end
		
	end
	
	------------------------------------------------------------
	--佣兵挑战物品奖励
	local MercItemTbl = {}
	if data["IsGiveMercItem"] then
--		if DBLevel >= 10 and DBLevel < 20 then
--			local ItemName = RandomGetStone()
--			if ItemName then
--				table.insert(MercItemTbl, {38,ItemName,1})
--			end
--		elseif DBLevel >= 20 then--20级后产出
--			local ItemName = RandomGetMercItem(DBPlayerYbLevel,data["PlayerCamp"])
--			if ItemName then
--				table.insert(MercItemTbl, {1,ItemName,2})
--			end
--		end
		if DBLevel >= 20 then
			table.insert(MercItemTbl,{24,"佣兵任务宝箱",5})
		end
	end
	if data["IsGiveScopesItem"] then
		if DBLevel >= 55 then--55级后产出
			local ItemName = RandomGetScopesItem(data["PlayerCamp"])
			if ItemName then
				table.insert(MercItemTbl, {1,ItemName,3})
			end
		end
	end
	
	for i=1, #(MercItemTbl) do
		local OtherItemType = MercItemTbl[i][1]
		local OtherItemName = MercItemTbl[i][2]
		local MsgID = MercItemTbl[i][3]
		
		local params= {}
			params.nCharID = PlayerId
			params.nStoreMain = g_StoreRoomIndex.PlayerBag
			params.nBigID = OtherItemType
			params.nIndex = OtherItemName
			params.nCount = 1
			params.uEventId = uEventId
		
		local res = g_RoomMgr.AddItem(params)
		if IsNumber(res) then
			CancelTran()
			return false,res
		end
		table.insert(res_tbl, {OtherItemType,OtherItemName,1,res,MsgID})
	end
	---------------------------------------------------------------
	
	--佣兵卡牌奖励
	if g_AllMercenaryQuestMgr[sQuestName] and QuestNode("佣兵卡牌奖励") then
		local MercenaryType = QuestNode("佣兵任务类型")
		if MercenaryType >= 2 and MercenaryType <= 4 then
			local Merc_give = GetCfgTransformValue(true, "Quest_Common", sQuestName, "佣兵卡牌奖励")
			for i=1,table.getn(Merc_give) do
				local itemtype,name,num, BindingStyle, Rate=Merc_give[i][1],Merc_give[i][2],Merc_give[i][3],Merc_give[i][4],Merc_give[i][5]
				local randnum = math.random(1,100)
				if randnum <= (Rate*100) then
					local params= {}
					params.nCharID = PlayerId
					params.nStoreMain = g_StoreRoomIndex.PlayerBag
					params.nBigID = itemtype
					params.nIndex = name
					params.nCount = num
					params.BindingType = BindingStyle
					params.uEventId = uEventId
					local res = g_RoomMgr.AddItem(params)
					if IsNumber(res) then
						CancelTran()
						return false,res
					end
					table.insert(res_tbl,{itemtype,name,num,res,4})
				end
			end
		end
	end
	
	local countstr = QuestNode("金钱奖励")
	local money_info = {}
	--金钱奖励
	--服务器端  奖励
	if countstr and countstr~="" then
		local count = CountReturn(DBLevel,DBPlayerYbLevel,LoopNum,countstr)
		local fun_info = g_MoneyMgr:GetFuncInfo("Quest")
		local bFlag,uMsgID = MoneyManagerDB.AddMoney(fun_info["FunName"],fun_info["MoneyAward"],PlayerId,count,uEventId)
		if not bFlag then
			CancelTran()
			if IsNumber(uMsgID) then
				return false, uMsgID
			else
				return false, 3094 --"金钱奖励失败"
			end
		end
		table.insert(money_info, {count, 1})
	end
	
	--绑定的金钱奖励
	countstr = QuestNode("绑定的金钱奖励")
	if countstr and countstr~="" then
		local count = CountReturn(DBLevel,DBPlayerYbLevel,LoopNum,countstr)
		local fun_info = g_MoneyMgr:GetFuncInfo("Quest")
		local bFlag,uMsgID = MoneyManagerDB.AddMoneyByType(fun_info["FunName"],fun_info["AddBindingMoney"],PlayerId, 2, count,uEventId)
		if not bFlag then
			CancelTran()
			if IsNumber(uMsgID) then
				return false,uMsgID
			else
				return false,3095
			end
		end
		table.insert(money_info, {count, 2})
	end
	
	--绑定的元宝奖励
	countstr = QuestNode("绑定的元宝奖励")
	if countstr and countstr~="" then
		local count = CountReturn(DBLevel,DBPlayerYbLevel,LoopNum,countstr)
		local fun_info = g_MoneyMgr:GetFuncInfo("Quest")
		local bFlag,uMsgID = MoneyManagerDB.AddMoneyByType(fun_info["FunName"],fun_info["AddBindingTicket"],PlayerId, 4, count,uEventId)
		if not bFlag then
			CancelTran()
			if IsNumber(uMsgID) then
				return false,uMsgID
			else
				return false,3096
			end
		end
		table.insert(money_info, {count, 3})
	end 
	
	--声望奖励
	countstr = QuestNode("声望奖励")
	if countstr and countstr~="" then
		local honor_count = 0
		honor_count = CountReturn(DBLevel,DBPlayerYbLevel,LoopNum,countstr)
		--任务完成加荣誉
		LogMgr.AddHonorByEventId(uEventId,honor_count)
	end
	
	--帮会荣誉奖励
	countstr = QuestNode("帮会荣誉奖励")
	if TongId ~= 0 and countstr and countstr~="" then
		local count = CountReturn(DBLevel,DBPlayerYbLevel,LoopNum,countstr)
		--任务完成加荣誉
		local parameters = {}
		parameters["uTongID"] = TongId
--		parameters["uHonor"] = count
		parameters["uExploit"] = count
		parameters["nEventType"] = 144
--		GasTongBasicDB.UpdateHonor(parameters)
		--GasTongBasicDB.UpdateExploit(parameters)
	end
	
	--帮会团贡奖励
	countstr = QuestNode("帮会团贡奖励")
	if countstr and countstr~="" then
		local count = CountReturn(DBLevel,DBPlayerYbLevel,LoopNum,countstr)
		AreaFbPointDB.AddAreaFbPointByType(PlayerId,count,10,sceneName,nil,event_type_tbl["任务帮会团贡奖励"])
	end
	
	--绑定的佣兵券奖励
	countstr = QuestNode("绑定的佣兵券奖励")
	if countstr and countstr~="" then
		local count = CountReturn(DBLevel,DBPlayerYbLevel,LoopNum,countstr)
		local fun_info = g_MoneyMgr:GetFuncInfo("Quest")
		local bFlag,uMsgID = MoneyManagerDB.AddMoneyByType(fun_info["FunName"],fun_info["AddBindingTicket"],PlayerId, 3, count,uEventId)
		if not bFlag then
			CancelTran()
			if IsNumber(uMsgID) then
				return false,uMsgID
			else
				return false,3105
			end
		end
	end 
	
	--发展度奖励
	countstr = QuestNode("发展度奖励")
	if countstr and countstr~="" and TongId~=0 then
		local count = CountReturn(DBLevel,DBPlayerYbLevel,LoopNum,countstr)
		local bFlag = GasTongBasicDB.AddTongDevelopDegree{["uTongID"] = TongId, ["uPoint"] = count, ["uEventType"] = event_type_tbl["任务加发展度奖励"]}
		if not bFlag then
			CancelTran()
			return false,3107
		end
	end
	
	--经验奖励
	local NowLevel = DBLevel
	
	local AddExp,AddSoul = CountAddExpAndSoulNum(DBLevel,DBPlayerYbLevel,LoopNum,sQuestName)

--	print("AddExp,AddSoul",AddExp,AddSoul)
	AddExp = AddExp * uExpMultiple
	local CurLevel,LevelExp = AddPlayerExp(DBLevel,AddExp,DBExp,PlayerId)
	local AddExpTbl = nil
	if CurLevel then
		NowLevel = CurLevel
		data["char_level"]	= CurLevel
		data["char_exp"]	= LevelExp
		data["nAddExp"] = AddExp
		data["uEventId"] = uEventId
		CharacterMediatorDB.AddLevel(data)
		
		AddExpTbl = {}
		AddExpTbl["Level"] = CurLevel
		AddExpTbl["Exp"] = LevelExp
		AddExpTbl["AddExp"] = AddExp
		AddExpTbl["uInspirationExp"] = 0
	end
	
	local MercenaryLevelTbl = nil
	if g_AllMercenaryQuestMgr[sQuestName] then
		local AddIntegral = GetMercenaryIntegralNum(DBLevel,DBPlayerYbLevel,LoopNum,sQuestName)
		local CurrMercenaryLevel,CurrIntegral = AddPlayerMercenaryIntegral(DBPlayerYbLevel,AddIntegral,DBIntegral)
		if CurrMercenaryLevel then
			local AddLevelParam = {}
			AddLevelParam["char_id"] = PlayerId
			AddLevelParam["char_mercLevel"]	= CurrMercenaryLevel
			AddLevelParam["char_mercIntegral"]	= CurrIntegral
			CharacterMediatorDB.AddMercenaryLevel(AddLevelParam)
			MercenaryLevelTbl = {CurrMercenaryLevel,CurrIntegral,AddIntegral}
		end
--	else		--其它的任务,给4个积分
--		
--		local AddLevelParam = {}
--		AddLevelParam["char_id"] = PlayerId
--		AddLevelParam["char_mercLevel"]	= DBPlayerYbLevel
--		AddLevelParam["char_mercIntegral"]	= DBIntegral+4
--		CharacterMediatorDB.AddMercenaryLevel(AddLevelParam)
--		MercenaryLevelTbl = {DBPlayerYbLevel,DBIntegral+4,4}
		
	end
	
	local parm = {}
	parm["soulCount"] = AddSoul
	parm["PlayerId"] = PlayerId
	parm["eventId"] = uEventId
	local resSucc,SoulRet = EquipMgrDB.ModifyPlayerSoul(parm)
	if not resSucc then
		CancelTran()
		return false
	end
	if not NowLevel and NowLevel < DBLevel then
		NowLevel = DBLevel
	end
	--返回客户端
	--g_MoneyMgr:Flush()
	return true,res_tbl,AddExpTbl,SoulRet,MercenaryLevelTbl,AddSoul
end

--给予任务奖励
function RoleQuestDB.GiveQuestAward(data,uExpMultiple,sceneName,LoopNum)
	return GiveQuestAward(data,uExpMultiple,sceneName,LoopNum)
end

local function FinishQuest(data)
	local PlayerId = data["PlayerId"]
	local sQuestName = data["QuestName"]
	local sceneName = data["sceneName"]
	local QuestNode = Quest_Common(sQuestName)
	
	if RoleQuestDB.CountQuestLimitByName(sQuestName) > 0 then
		return {false, 3103}
	end
	
	--检查服务器端该任务状态
	local state, IsLimitTime=RoleQuestDB.GetQuestState({PlayerId,sQuestName})
	if(state ~= QuestState.init) then
		return {false, 3097} --"不是进行中任务"
	end
--	if(state == QuestState.finish) then
--		return {false,"任务已经完成"}
--	end
	
	--如计时器任务，是否超时
	if IsLimitTime ~= 1 then
		local limit_time = QuestNode("限时")
		if limit_time and limit_time~=0 then
			local start_time = RoleQuestDB.GetQuestAcceptTime(sQuestName,PlayerId)
	--		if(start_time == nil) then
	--			return {false,"没有开始时间,没有接受任务"}
	--		end
			local finish_time = os.time()
			local dtime = (finish_time-start_time)
			if(dtime > limit_time) then
				return {false, 3098} --"任务超时"
			end
		end
	end
	
	--检查任务是否满足需求
	if g_QuestNeedMgr[sQuestName] then
		local vartbl = RoleQuestDB.SelectQuestVarList(sQuestName,PlayerId)
		for i = 1, table.getn(vartbl) do
			local varname = vartbl[i][1]
			local donum = vartbl[i][2]
			local neednum = g_QuestNeedMgr[sQuestName][varname].Num
			if donum < neednum then
				return {false, 3099} --"任务需求尚未满足"
			end
		end
	end
	
	if QuestNode("佣兵团团长限制") and QuestNode("佣兵团团长限制") == 1 then
		local GasTongBasicDB = RequireDbBox("GasTongBasicDB")
		local TongId = GasTongBasicDB.GetTongID(PlayerId)
		if TongId == 0 or PlayerId ~= GasTongBasicDB.GetTongLeaderId(TongId) then
			return {false, 300026}
		end
	end
	
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local Lev_Exp = CharacterMediatorDB.GetCharLevelExpDB({["char_id"] = PlayerId})
	local DBLevel = Lev_Exp:GetData(0,0)
	--local MercenaryLevelDB = RequireDbBox("MercenaryLevelDB")
	--MercenaryLevelDB.SlowAddMercenaryLevelCount(PlayerId, "挑战试炼", sQuestName)
	--如果是佣兵任务
	local FinishTimesTbl = {}
	if g_AllMercenaryQuestMgr[sQuestName] then
		local MercenaryType = QuestNode("佣兵任务类型")
		
		if MercenaryType == 7 and DBLevel >= 15 then
			local mercParam = {}
			mercParam["PlayerId"] = PlayerId
			mercParam["ActivityName"] = "佣兵小队任务"
			local ActivityCountDbBox = RequireDbBox("ActivityCountDB")
			local IsAllow, FinishTimes = ActivityCountDbBox.CheckSinglePlayerActivityCount(mercParam)
			
			FinishTimesTbl[1] = FinishTimes + 1
			if IsAllow then
				ActivityCountDbBox.AddActivityCount(mercParam)
			end
		end
		
		--如果是佣兵日常任务,检查任务是否已经完成了10个
		if MercenaryType == 1 and DBLevel >= 20 then
			local mercParam = {}
			mercParam["PlayerId"] = PlayerId
			mercParam["ActivityName"] = "佣兵日常任务"
			local ActivityCountDbBox = RequireDbBox("ActivityCountDB")
			local IsAllow, FinishTimes = ActivityCountDbBox.CheckSinglePlayerActivityCount(mercParam)
			
			FinishTimesTbl[2] = FinishTimes + 1
			--if not IsAllow then
				--已经够了10个了,不能再交佣兵日常任务
				--return {false,"你已经够了10个了,不能再交佣兵日常任务"}
			if IsAllow then
				ActivityCountDbBox.AddActivityCount(mercParam)
				--随机给物品奖励(第2次的时候给奖励)
				FinishTimes = FinishTimes + 1
				
				if FinishTimes == 10 or FinishTimes == 20 then
					data["IsGiveMercItem"] = true
				end
				if FinishTimes/2 == math.floor(FinishTimes/2) then
					--data["IsGiveMercItem"] = true
					--data["IsGiveScopesItem"] = true
				end
			end
		end
		--低速加佣兵等级指引计数
		--MercenaryLevelDB.SlowAddMercenaryLevelCount(PlayerId, "佣兵任务")
	end
	
	local del_items={}
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	
	if QuestNode("物品需求") then
		local Keys = QuestNode:GetKeys("物品需求")
		for k = 1, table.getn(Keys) do
			local Arg = GetCfgTransformValue(true, "Quest_Common", sQuestName, "物品需求", Keys[k], "Arg")
			local needtype = Arg[1]
			local needname = Arg[2]		--需求的物品名
			local neednum = Arg[3]
			local have_num = g_RoomMgr.GetItemCount(PlayerId,needtype,needname)
			if have_num < neednum then
				CancelTran()
				return {false, 3099} --"任务需求尚未满足"
			elseif needtype ~=16 and have_num>=neednum then
				local res = g_RoomMgr.DelItem(PlayerId, needtype,needname,neednum,nil,event_type_tbl["任务品或者道具物品删除"])
				if IsNumber(res) then
					CancelTran()
					return {false,3089} --"需求物品删除失败"
				end
				table.insert(del_items,{needtype,needname,neednum,res})
			else
				local res = g_RoomMgr.DelItem(PlayerId, needtype,needname,have_num,nil,event_type_tbl["任务品或者道具物品删除"])
				if IsNumber(res) then
					CancelTran()
					return {false,3089} --"需求物品删除失败"
				end
				table.insert(del_items,{needtype,needname,have_num,res})
			end
		end
	end
	
	if QuestNode("开启掉落") then
		local Keys = QuestNode:GetKeys("开启掉落")
		for k = 1, table.getn(Keys) do
			local Arg = GetCfgTransformValue(true, "Quest_Common", sQuestName, "开启掉落", Keys[k], "Arg")
			local droptype = Arg[1]
			local dropname = Arg[2]		--需求的物品名
			local havenum = g_RoomMgr.GetItemCount(PlayerId,droptype,dropname)
			if havenum > 0 then
				local res = g_RoomMgr.DelItem(PlayerId, droptype,dropname,havenum,nil,event_type_tbl["任务品或者道具物品删除"])
				if IsNumber(res) then
					CancelTran()
					return {false,3089} --"需求物品删除失败"
				end
				table.insert(del_items,{droptype,dropname,havenum,res})
			end
		end
	end
	
	--加时间限次计数
	local GMTimeCountLimit = data["GMTimeCountLimit"]
	if QuestNode("时间限次") and GMTimeCountLimit == 1 then
		AddQuestTimeCount(sQuestName,PlayerId)
	end
	--清空任务变量
	if not RoleQuestDB.DeleteAllQuestVar(sQuestName,PlayerId) then
		CancelTran()
		return {false,3090} --"清空任务变量失败"
	end
	--设置任务状态
	if not RoleQuestDB.UpdateQuestState(QuestState.finish,PlayerId,sQuestName) then
		CancelTran()
		return {false,300005} --"设置任务状态失败"
	end
	
	if not RoleQuestDB.UpdateCharFinishQuestNum(PlayerId,1) then
		CancelTran()
		return {false,3091} --"增加任务完成数量失败"
	end
	--清空任务道具
	if QuestNode("获得物品") then
		local Keys = QuestNode:GetKeys("获得物品")
		for i = 1,table.getn(Keys) do
			local tbl = GetCfgTransformValue(true, "Quest_Common", sQuestName, "获得物品", Keys[i])
			if tbl[3] then
				local give_type = tbl[1]
				local give_name = tbl[2]
				local del_num = g_RoomMgr.GetItemCount(PlayerId,give_type,give_name)
				if(del_num>0) then
					local res = g_RoomMgr.DelItem(PlayerId, give_type, give_name,del_num,nil,event_type_tbl["任务品或者道具物品删除"])
					if IsNumber(res) then
						CancelTran()
						return {false,res}
					end
					table.insert(del_items,{give_type,give_name,del_num, res})
				end
			end
		end
	end
	
	local LoopQuestTbl = {"", 0}
	local loopname, loopnum = "", 0
	if Quest_Common(sQuestName, "任务类型") == 10 then
		local tbl = GetCfgTransformValue(true, "Quest_Common", sQuestName, "环属性", "1")
		local res = StmtContainer._GetQuestLoop:ExecStat(PlayerId, tbl[1])
		if res and res:GetRowNum()>0 then
			local PlayerLevel = data["PlayerLevel"]
			loopname = tbl[1]
			loopnum = res:GetData(0, 0)
			local loopmgr = g_LoopQuestMgr[loopname][loopnum+1]
			local n = 0
			local temptbl = {}
			if loopmgr then
				for i=1, table.getn(loopmgr) do
					if sQuestName ~= loopmgr[i].QuestName
						and PlayerLevel >= loopmgr[i].MinLevel and PlayerLevel <= loopmgr[i].MaxLevel then
						table.insert(temptbl, loopmgr[i].QuestName)
					end
				end
				n = table.getn(temptbl)
				if n > 0 then
					LoopQuestTbl[1] = temptbl[math.random(1, n)]
					LoopQuestTbl[2] = loopnum+1
				end
			end
			if n <= 0 then
				StmtContainer._DeleteQuestLoop:ExecStat(PlayerId, loopname)
				LoopQuestTbl[1] = "LoopOver"
				LoopQuestTbl[2] = 0
			end
		end
	end
	
	local uExpMultiple = RoleQuestDB.GetQuestExpMultiple(PlayerId,sQuestName)
	local succ,add_items,AddExpTbl,SoulRet,MercenaryLevelTbl,uAddSoul = RoleQuestDB.GiveQuestAward(data,uExpMultiple,sceneName,loopnum)
	if not succ then
		CancelTran()
		return {false,add_items}
	end
	
	local delItemTbl = {}
	for i=1,table.getn(del_items) do
		local type,name,num,res=del_items[i][1],del_items[i][2],del_items[i][3],del_items[i][4]
		local Tbl = {}
		Tbl["nType"] = type
		Tbl["sName"] = name
		Tbl["nCount"] = num
		Tbl["res"] = res
		table.insert(delItemTbl, Tbl)
	end
	
	local DirectDB = RequireDbBox("DirectDB")
	local DirectTbl = DirectDB.AddPlayerQuestDirect(PlayerId, "完成任务", sQuestName)
	
	local addItemTbl = {}
--	local param = {}
--	param["char_id"] = PlayerId
	for i=1, table.getn(add_items) do
--		param["nType"] = add_items[i][1]
--		param["sName"] = add_items[i][2]
--		param["nCount"] = add_items[i][3]
--		param["itemTbl"] = add_items[i][4]
		local Tbl = {}
		--Tbl["QuestItemVar"] = RoleQuestDB.AddQuestItemVar(param)
		Tbl["nType"] = add_items[i][1]
		Tbl["sName"] = add_items[i][2]
		Tbl["nCount"] = add_items[i][3]
		Tbl["res"] = add_items[i][4]
		Tbl["msgType"] = add_items[i][5]
		DirectDB.AddPlayerItemDirect(PlayerId, "任务获得物品", Tbl["nType"], Tbl["sName"], DirectTbl)
		table.insert(addItemTbl, Tbl)
	end
	
	local TeamPlayerID = data["TeamPlayerID"]
	local TeamPlayerExp = nil
	if TeamPlayerID and next(TeamPlayerID) then
		if AddExpTbl and AddExpTbl["AddExp"] then
			local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
			local AreaDB = RequireDbBox("AreaDB")
			TeamPlayerExp = {}
			for i=1, #(TeamPlayerID) do
				local iExp = math.floor(AddExpTbl["AddExp"]*0.05)
				local ID = TeamPlayerID[i]
				
				if AreaDB.Check2PlayerInSameArea(PlayerId,ID) then
					local param = {}
					param["char_id"] = ID
					local Lev_Exp = CharacterMediatorDB.GetCharLevelExpDB(param)
					local iDBLevel = Lev_Exp:GetData(0,0)
					local iDBExp = Lev_Exp:GetData(0,1)
					Lev_Exp:Release()
					
					local iCurLevel,iLevelExp = AddPlayerExp(iDBLevel,iExp,iDBExp,ID)
					if iCurLevel then
						param["char_level"]	= iCurLevel
						param["char_exp"]	= iLevelExp
						param["nAddExp"] = iExp
						param["addExpType"] = event_type_tbl["队友任务经验分享"]
						CharacterMediatorDB.AddLevel(param)
						
						local iAddExpTbl = {}
						iAddExpTbl["Level"] = iCurLevel
						iAddExpTbl["Exp"] = iLevelExp
						iAddExpTbl["AddExp"] = iExp
						iAddExpTbl["uInspirationExp"] = 0
						iAddExpTbl["DirectTbl"] = DirectTbl
						table.insert(TeamPlayerExp,{ID,iAddExpTbl})
					end
				end
				
			end
		end
	end
	
	--最后检查一下佣兵等级状态，因为人物升级，以及普通任务给佣兵积分也会影响佣兵等级状态
	--local MLRes = MercenaryLevelDB.CheckMercenaryLevel( {["CharId"] = PlayerId} )
	
	local resData = {}
	resData["delItemTbl"] = delItemTbl
	resData["addItemTbl"] = addItemTbl
	resData["AddExpTbl"] = AddExpTbl
	resData["SoulRet"] = SoulRet
	resData["MercenaryLevelTbl"] = MercenaryLevelTbl
	resData["TeamPlayerExp"] = TeamPlayerExp
	--resData["MLRes"] = MLRes
	resData["DirectTbl"] = DirectTbl
	resData["LoopQuestTbl"] = LoopQuestTbl
	resData["uAddSoul"] = uAddSoul
	
	if LoopQuestTbl[1] ~= "" and LoopQuestTbl[1] ~= "LoopOver" then
		local param = {}
		param["PlayerId"] = PlayerId
		param["QuestName"] = LoopQuestTbl[1]
		param["sceneName"] = sceneName
		param["LoopNum"] = LoopQuestTbl[2]
		resData["AddLoopQuestResult"] = AddQuest(param)
	end
	local buffName = Quest_Common(sQuestName, "buff奖励")
	if buffName and vipBuffTbl[buffName] then
		local UseVIPItemDB = RequireDbBox("UseVIPItemDB")
		local succ = UseVIPItemDB.SaveWelfareRole(PlayerId,buffName)
		if not succ then
			CancelTran()
			return {false,3091}
		end
	end
	return {true,resData}
end

--完成任务
function RoleQuestDB.FinishQuest(data)
	return FinishQuest(data)
end

local function GiveUpQuest(data)
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local PlayerId = data["PlayerId"]
	local sQuestName = data["QuestName"]
	local sceneName = data["sceneName"]
	local del_items = {}
	
	if Quest_Common(sQuestName, "获得物品") then
		local Keys = Quest_Common:GetKeys(sQuestName, "获得物品")
		for i = 1,table.getn(Keys) do
			local tbl = GetCfgTransformValue(true, "Quest_Common", sQuestName, "获得物品", Keys[i])
			if tbl[3] then
				local give_type = tbl[1]
				local give_name = tbl[2]
				local give_num = tonumber(tbl[3])
				local num = g_RoomMgr.GetItemCount(PlayerId, give_type, give_name)
				if(num ~= 0) then
					if(give_num >= num) then
						local res = g_RoomMgr.DelItem(PlayerId, give_type,give_name,num,nil,event_type_tbl["任务品或者道具物品删除"])
						if IsNumber(res) then
							CancelTran()
							return {false,res}
						end
						table.insert(del_items,{give_type,give_name,num, res})
					else
						local res = g_RoomMgr.DelItem(PlayerId, give_type,give_name,give_num,nil,event_type_tbl["任务品或者道具物品删除"])
						if IsNumber(res) then
							CancelTran()
							return {false,res}
						end
						table.insert(del_items,{give_type,give_name,give_num, res})
					end
				end
			end
		end
	end
	
	--刷新任务冷却时间
	local cdtime = Quest_Common(sQuestName, "冷却时间")
	if cdtime then
		RefreshQuestCD(sQuestName, PlayerId)
	end
	
	if Quest_Common(sQuestName, "任务类型") == 10 then
		local tbl = GetCfgTransformValue(true, "Quest_Common", sQuestName, "环属性", "1")
		local res = StmtContainer._GetQuestLoop:ExecStat(PlayerId, tbl[1])
		if res and res:GetRowNum()>0 then
			StmtContainer._DeleteQuestLoop:ExecStat(PlayerId, tbl[1])
		end
	end
	
	data["item_tbl"] = del_items
	return RoleQuestDB.DeleteQuest(data)
end

--放弃任务
function RoleQuestDB.GiveUpQuest(data)
	return GiveUpQuest(data)
end

local function CheckQuestState(PlayerId, sQuestName)
	--如计时器任务，是否超时
	local limit_time = Quest_Common(sQuestName, "限时")
	if limit_time and limit_time~=0 then
		local start_time = RoleQuestDB.GetQuestAcceptTime(sQuestName,PlayerId)
--		if(start_time == nil) then
--			return {false,"没有开始时间,没有接受任务"}
--		end
		local finish_time = os.time()
		local dtime = (finish_time-start_time)
		if(dtime > limit_time) then
			return false--"任务超时"
		end
	end
	
	--检查任务是否满足需求
	if g_QuestNeedMgr[sQuestName] then
		local vartbl = RoleQuestDB.SelectQuestVarList(sQuestName,PlayerId)
		for i = 1, table.getn(vartbl) do
			local varname = vartbl[i][1]
			local donum = vartbl[i][2]
			local neednum = g_QuestNeedMgr[sQuestName][varname].Num
			if donum < neednum then
				return false--"任务需求尚未满足"
			end
		end
	end
	
	if Quest_Common(sQuestName, "物品需求") then
		local Keys = Quest_Common:GetKeys(sQuestName, "物品需求")
		local RoomMgr = RequireDbBox("GasRoomMgrDB")
		for k = 1, table.getn(Keys) do
			local Arg = GetCfgTransformValue(true, "Quest_Common", sQuestName, "物品需求", Keys[k], "Arg")
			local needtype = Arg[1]
			local needname = Arg[2]		--需求的物品名
			local neednum = Arg[3]
			local have_num = RoomMgr.GetItemCount(PlayerId,needtype,needname)
			if have_num < neednum then
				return false--"任务需求尚未满足"
			end
		end
	end
	
	return true--都已经满足了
end

local function QuestFailure(data)
	local PlayerId = data["PlayerId"]
	local QuestName = data["QuestName"]
	
	--检查服务器端该任务状态
	local state,_ = RoleQuestDB.GetQuestState({PlayerId,QuestName})
	if not state then
		return false		--"不是进行中任务"
	end
	
	if CheckQuestState(PlayerId,QuestName) then--已经满足的话,就不设置为失败了
		return false
	end
	
	if not RoleQuestDB.UpdateQuestState(QuestState.failure,PlayerId, QuestName) then
		return false
	end
	local LogMgr = RequireDbBox("LogMgrDB")
	LogMgr.QuestFail(PlayerId, QuestName)
	return true
end

--任务失败
function RoleQuestDB.QuestFailure(data)
	return QuestFailure(data)
end

local function GutQuestFailure(data)
	local PlayerIdTbl = data["PlayerIdTbl"]
	local QuestName = data["QuestName"]
	
	for _, PlayerId in pairs(PlayerIdTbl) do 
		if CheckQuestState(PlayerId,QuestName) then--已经满足的话,就不设置为失败了
			return false
		end
		if not RoleQuestDB.UpdateQuestState(QuestState.failure,PlayerId, QuestName) then
			return false
		end
		local LogMgr = RequireDbBox("LogMgrDB")
		LogMgr.QuestFail(PlayerId, QuestName)	
	end
	return true
end

--剧情任务失败
function RoleQuestDB.GutQuestFailure(data)
	return GutQuestFailure(data)
end

function RoleQuestDB.AllPlayerQuestFail(data)
	local PlayerTbl = data["PlayerIdTbl"]
	local QuestName = data["QuestName"]
	local res = {}
	for _,id in pairs(PlayerTbl) do
		local param = {}
		param["PlayerId"] = id
		param["QuestName"] = QuestName
		table.insert(res,{id,RoleQuestDB.QuestFailure(param)})
	end
	return res
end

--离开场景,让任务失败
function RoleQuestDB.LeaveSceneSetQuestFailure(data)
	local PlayerId = data["char_id"]
	local SceneName = data["scene_name"]
	if data["scene_type"] >= 2 then	--其他副本
		local QuestNumRes = StmtContainer._GetDoingQuests:ExecStat(PlayerId)
		if QuestNumRes:GetRowNum() > 0 then
			local FailureQuestTbl = {}
			local param = {}
			for i=1, QuestNumRes:GetRowNum() do
				local sQuestName = QuestNumRes:GetData(i-1,0)
				if Quest_Common(sQuestName) then
					local Quest = Quest_Common(sQuestName, "离开场景任务失败")
					if Quest and Quest == SceneName then
						param["PlayerId"] = PlayerId
						param["QuestName"] = sQuestName
						table.insert(FailureQuestTbl,{["DelSucc"]=RoleQuestDB.QuestFailure(param),["QuestName"]=sQuestName})
					end
				end
			end
			return FailureQuestTbl
		end
	end
end

local function DeleteQuest(data)
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local PlayerId = data["PlayerId"]
	local sQuestName = data["QuestName"]
	local del_items = data["item_tbl"]
	local sceneName = data["sceneName"]
	--清空任务变量
	if not RoleQuestDB.DeleteAllQuestVar(sQuestName,PlayerId) then
		CancelTran()
		return {false,3090} --"清空任务变量失败"
	end
	--删除任务
	if not RoleQuestDB.GiveUpQuestSql(sQuestName, PlayerId) then
		CancelTran()
		return {false,300011} --"删除任务失败"
	end
	
	if Quest_Common(sQuestName, "开启掉落") then
		local Keys = Quest_Common:GetKeys(sQuestName, "开启掉落")
		for k = 1, table.getn(Keys) do
			local Arg = GetCfgTransformValue(true, "Quest_Common", sQuestName, "开启掉落", Keys[k], "Arg")
			local droptype = Arg[1]
			local dropname = Arg[2]
			local havenum = g_RoomMgr.GetItemCount(PlayerId,droptype,dropname)
			if havenum > 0 then
				local res = g_RoomMgr.DelItem(PlayerId, droptype,dropname,havenum,nil,event_type_tbl["任务品或者道具物品删除"])
				if IsNumber(res) then
					CancelTran()
					return {false,3089} --"需求物品删除失败"
				end
				table.insert(del_items,{droptype,dropname,havenum,res})
			end
		end
	end
	
	if Quest_Common(sQuestName, "物品需求") then
		local Keys = Quest_Common:GetKeys(sQuestName, "物品需求")
		local RoomMgr = RequireDbBox("GasRoomMgrDB")
		for k = 1, table.getn(Keys) do
			local Arg = GetCfgTransformValue(true, "Quest_Common", sQuestName, "物品需求", Keys[k], "Arg")
			local needtype = Arg[1]
			local needname = Arg[2]		--需求的物品名
			local neednum = Arg[3]
			local have_num = g_RoomMgr.GetItemCount(PlayerId,needtype,needname)
--			if needtype ~= 16 and have_num>=neednum then
--				local res = g_RoomMgr.DelItem(PlayerId, needtype,needname,neednum)
--				if IsNumber(res) then
--					CancelTran()
--					return {false,"删除任务物品失败"}
--				end
--				if IsTable(res) then
--					for i = 1,#res do
--						for j = 1,#res[i] do
--							g_RoomMgr.DelItemFromStaticTable(res[i][j])
--						end
--					end
--				end
--				table.insert(del_items,{needtype,needname,neednum,res})
--			elseif have_num>0 then
			if have_num>0 and needtype == 16 then
				local res = g_RoomMgr.DelItem(PlayerId, needtype,needname,have_num,nil,event_type_tbl["任务品或者道具物品删除"])
				if IsNumber(res) then
					CancelTran()
					return {false, 300012} --"删除任务物品失败"
				end
				table.insert(del_items,{needtype,needname,have_num,res})
			end
		end
	end
	
	local delItemTbl = {}
	for i=1,table.getn(del_items) do
		local type,name,num,res=del_items[i][1],del_items[i][2],del_items[i][3],del_items[i][4]
		local Tbl = {}
		Tbl["nType"] = type
		Tbl["sName"] = name
		Tbl["nCount"] = num
		Tbl["res"] = res
		table.insert(delItemTbl, Tbl)
	end
	
	local LogMgr = RequireDbBox("LogMgrDB")
	LogMgr.GiveUpQuest(PlayerId, sQuestName)
	
	return {true,delItemTbl}
end

--清除任务相关信息(以及相关的物品信息)
function RoleQuestDB.DeleteQuest(data)
	return DeleteQuest(data)
end

local function DecreQuestItemVar(data)
	local PlayerId = data["PlayerId"]
	local ItemType = data["ItemType"]
	local ItemName = data["ItemName"]
	local nCount = data["nCont"]
	local sceneName = data["sceneName"]
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	
--	local ChangeQuestItemTbl = {}
--	if g_ItemQuestMgr[ItemName] then
--		for i = 1, table.getn(g_ItemQuestMgr[ItemName]) do
--			if ItemType == g_ItemQuestMgr[ItemName][i][1] then
--				local sQuestName = g_ItemQuestMgr[ItemName][i][2]
--				local ItemTbl = GetItemVarNum(PlayerId,sQuestName, ItemType, ItemName)
--				table.insert(ChangeQuestItemTbl,ItemTbl)
--			end
--		end
--	end
	--local DelQuestItemTbl = {}
	--local DelGiveQuestItemTbl = {}
	
	if g_WhereGiveQuestMgr["Goods"][ItemName] then
		local sQuestName = g_WhereGiveQuestMgr["Goods"][ItemName][1]
		local queststate = RoleQuestDB.GetQuestState({PlayerId,sQuestName})
		if queststate ~= nil and queststate ~= QuestState.finish then
			local del_items = {}
			if Quest_Common(sQuestName, "获得物品") then
				local Keys = Quest_Common:GetKeys(sQuestName, "获得物品")
				for i = 1,table.getn(Keys) do
					local tbl = GetCfgTransformValue(true, "Quest_Common", sQuestName, "获得物品", Keys[i])
					if tbl[3] then
						local give_type = tbl[1]
						local give_name = tbl[2]
						local give_num = tonumber(tbl[3])
						local num = g_RoomMgr.GetItemCount(PlayerId, give_type, give_name)
						if(num ~= 0) then
							if(give_num >= num) then
								local res = g_RoomMgr.DelItem(PlayerId, give_type,give_name,num,nil,event_type_tbl["任务品或者道具物品删除"])
								if IsNumber(res) then
									CancelTran()
									return {false,res}
								end
								table.insert(del_items,{give_type,give_name,num, res})
							else
								local res = g_RoomMgr.DelItem(PlayerId, give_type,give_name,give_num,nil,event_type_tbl["任务品或者道具物品删除"])
								if IsNumber(res) then
									CancelTran()
									return {false,res}
								end
								table.insert(del_items,{give_type,give_name,give_num, res})
							end
						end
					end
				end
			end
			local param = {}
			param["PlayerId"] = PlayerId
			param["QuestName"] = sQuestName
			param["item_tbl"] = del_items
			param["sceneName"] = data["sceneName"]
			local result = RoleQuestDB.DeleteQuest(param)
			if not result[1] then
				return {false,result[2]}
			else
				return {true,sQuestName,result[2]}
				--DelQuestItemTbl = result[2]
			end
			--return {true,sQuestName,DelQuestItemTbl}
		end
	end
	
	--local DelQuestName = nil
	if g_QuestPropMgr[ItemName] 
		and g_QuestPropMgr[ItemName][1] == ItemType then
		local sQuestName = g_QuestPropMgr[ItemName][2]
		local queststate = RoleQuestDB.GetQuestState({PlayerId,sQuestName})
		if queststate ~= nil and queststate ~= QuestState.finish then
			local del_items = {}
			if Quest_Common(sQuestName, "获得物品") then
				local Keys = Quest_Common:GetKeys(sQuestName, "获得物品")
				for i = 1,table.getn(Keys) do
					local tbl = GetCfgTransformValue(true, "Quest_Common", sQuestName, "获得物品", Keys[i])
					if tbl[3] then
						local give_type = tbl[1]
						local give_name = tbl[2]
						local give_num = tonumber(tbl[3])
						local num = g_RoomMgr.GetItemCount(PlayerId, give_type, give_name)
						if(num ~= 0) then
							if(give_num >= num) then
								local res = g_RoomMgr.DelItem(PlayerId, give_type,give_name,num,nil,event_type_tbl["任务品或者道具物品删除"])
								if IsNumber(res) then
									CancelTran()
									return {false,res}
								end
								table.insert(del_items,{give_type,give_name,num, res})
							else
								local res = g_RoomMgr.DelItem(PlayerId, give_type,give_name,give_num,nil,event_type_tbl["任务品或者道具物品删除"])
								if IsNumber(res) then
									CancelTran()
									return {false,res}
								end
								table.insert(del_items,{give_type,give_name,give_num, res})
							end
						end
					end
				end
			end
			local param = {}
			param["PlayerId"] = PlayerId
			param["QuestName"] = sQuestName
			param["item_tbl"] = del_items
			param["sceneName"] = sceneName
			local result = RoleQuestDB.DeleteQuest(param)
			if not result[1] then
				return {false,result[2]}
			else
				return {true,sQuestName,result[2]}
				--DelQuestItemTbl = result[2]
			end
			--return {true,sQuestName,DelQuestItemTbl}
		end
	end
	return {true}
end

--删除任务物品,就什么删除相关的任务
function RoleQuestDB.DecreQuestItemVar(data)
	return DecreQuestItemVar(data)
end

local function DeleteItem(data)
	local PlayerId = data["PlayerId"]
	local ItemType = data["ItemType"]
	local ItemName = data["ItemName"]
	local ItemNum = data["nCont"]
	local sceneName = data["sceneName"]
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local res = g_RoomMgr.DelItem(PlayerId, ItemType, ItemName, ItemNum,nil,event_type_tbl["任务品或者道具物品删除"])
	if IsNumber(res) then
		CancelTran()
		return {false,300011} --"物品删除失败"
	end
	--加上这些,如果删除的是一个任务物品,那么任务也会删除
	--local result = RoleQuestDB.DecreQuestItemVar(data)
	--if not result[1] then
	--	return {false,result[2]}
	--end
	
	return {true,res}
end

--删除物品
function RoleQuestDB.DeleteItem(data)
	return DeleteItem(data)
end

local function DeleteItemByPos(data)
	local PlayerId 			= data["PlayerId"]
	local ItemType 			= data["ItemType"]
	local ItemName 			= data["ItemName"]
	local ItemNum				= data["nCont"]
	local ItemRoomIndex = data["nRoomIndex"]
	local ItemPos 			= data["nPos"]
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local DelRet = g_RoomMgr.DelItemByPos(PlayerId,ItemRoomIndex,ItemPos,ItemNum,event_type_tbl["任务品或者道具物品删除"])
	local res = {}
	if IsNumber(DelRet) then
		CancelTran()
		return {false,300011} --"物品删除失败"
	end
	table.insert(res,SetResult(DelRet,ItemRoomIndex,ItemPos,ItemNum,ItemType,ItemName))	
	--加上这些,如果删除的是一个任务物品,那么任务也会删除
	--local result = RoleQuestDB.DecreQuestItemVar(data)
	--if not result[1] then
	--	return {false,result[2]}
	--end
	
	--return {true,result[2],result[3],result[4],res}
	return {true,res}
end

--删除物品,(和物品位置有关)
function RoleQuestDB.DeleteItemByPos(data)
	return DeleteItemByPos(data)
end

function RoleQuestDB.ConsumeItem(data)
	local PlayerId = data["PlayerId"]
	local ConsumeTbl = data["ConsumeTbl"]
	local ConsumeItemTbl = {}
	for i,v in pairs(ConsumeTbl) do
		local UseTbl = v
		local ItemType = UseTbl[1]
		local ItemName = UseTbl[2]
		local ItemNum	= UseTbl[3]
		local result = {}
		data["ItemType"] = UseTbl[1]
		data["ItemName"] = UseTbl[2]
		data["nCont"] = UseTbl[3]
		
		if UseTbl[4] ~= nil then
			data["nRoomIndex"] = UseTbl[4]
			data["nPos"] = UseTbl[5]
			result = RoleQuestDB.DeleteItemByPos(data)
		else
			result = RoleQuestDB.DeleteItem(data)
		end
		
		if not result[1] then
			return {false,result[2]}
		end
		table.insert(ConsumeItemTbl,{ItemType,ItemName,ItemNum,result[2]})
	end
	return {true,ConsumeItemTbl}
end

local function AddExp(data)
	local AddExp = data["addExp"]
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local Lev_Exp = CharacterMediatorDB.GetCharLevelExpDB(data)
	local DBLevel = Lev_Exp:GetData(0,0)
	local DBExp = Lev_Exp:GetData(0,1)
	Lev_Exp:Release()
	local CurLevel,LevelExp = AddPlayerExp(DBLevel,AddExp,DBExp,data["char_id"])
	if CurLevel then
		data["char_level"] = CurLevel
		data["char_exp"] = LevelExp
		data["nAddExp"] = AddExp
		
		CharacterMediatorDB.AddLevel(data)
		return CurLevel,LevelExp
	end
end

--添加玩家经验
function RoleQuestDB.AddExp(data)
	return AddExp(data)
end

local function UseItemAddExp(data)
	data["addExpType"] = event_type_tbl["使用经验球获得经验"]
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local ItemType, ItemName, ItemCount = g_RoomMgr.GetTypeCountByPosition(data["nCharID"], data["nRoom"], data["nPos"])
	if ItemCount == nil or ItemCount == 0 or ItemType ~= 1 or ItemName ~= data["ItemName"] then
		return
	end

	local CurLevel,LevelExp = RoleQuestDB.AddExp(data)
	if CurLevel and LevelExp then
		local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
		local res = g_RoomMgr.DelItemByPos(data["nCharID"],data["nRoom"],data["nPos"],data["nCount"],event_type_tbl["物品使用"])
		if not res then
			CancelTran()
		end
		return res,{CurLevel,LevelExp}
	end
end

--经验球添加经验
function RoleQuestDB.UseItemAddExp(data)
	return UseItemAddExp(data)
end

local function SendItem(playerId, exp, fetch, money, itemTbl, MailTitle, MailContent,sceneName, isFlag, soul)
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local sysMailExecutor = RequireDbBox("SysMailDB")
	local AddItemsTbl = {}
	if itemTbl then
		for i=1, table.getn(itemTbl) do
			local item = itemTbl[i]
			local BigID	= item[1]
			local ItemName 	= item[2]
			local Count	= 0
			if isFlag then
				Count	=isFlag * item[3]
			else
				Count	= item[3]
			end
			
			local params= {}
			params.m_nType = BigID
			params.m_sName = ItemName
			params.m_nBindingType = g_ItemInfoMgr:GetItemInfo(BigID,ItemName,"BindingStyle") or 0
			params.m_nCharID = playerId
			params.m_sCreateType = 32
			for j=1, Count do
				local itemID = g_RoomMgr.CreateItem(params)
				if not itemID then
					return false
				end
				table.insert(AddItemsTbl,{itemID})
			end
		end
	end
	
	if AddItemsTbl[1] or (money and money~=0) then
 		local sendRet = sysMailExecutor.SendSysMail(1014, playerId, MailTitle, MailContent, AddItemsTbl, money, event_type_tbl["活动奖励邮件"])
		if sendRet == false then
			return false
		end
	end
	return true
end


local function PlayerAward(playerId, exp, fetch, money, itemTbl, MailTitle, MailContent,sceneName,sceneType, isFlag, soul)
	local expResult, soulResult, mailResult,bSucc, soulValue
	local  moneyResult = {}
	local itemResult = {}
	local data = {}
	data["char_id"] = playerId
	if exp and exp ~=0 then
		data["addExp"] = exp
		data["addExpType"] = event_type_tbl["副本奖励"]
		expResult = {AddExp(data)}
	end
	if soul and soul ~= 0 then
		local EquipMgrDB = RequireDbBox("EquipMgrDB")
		local parm = {}
		parm["soulCount"] = soul
		parm["PlayerId"] = playerId
		parm["addSoulType"] = event_type_tbl["副本奖励"]
		bSucc,soulValue = EquipMgrDB.ModifyPlayerSoul(parm)
		if not bSucc then
			return false
		end
	end 
	if fetch and fetch~=0 then
		local EquipMgrDB = RequireDbBox("EquipMgrDB")
		local parm = {}
		parm["soulCount"] = fetch
		parm["PlayerId"] = playerId
		parm["addSoulType"] = 32
		bSucc,soulResult = EquipMgrDB.ModifyPlayerSoul(parm)
		if not bSucc then
			return false
		end
	end
	if money and money ~= 0 then
		local MoneyManagerDB =	RequireDbBox("MoneyMgrDB")
		local fun_info = g_MoneyMgr:GetFuncInfo("Quest")
		
		local bFlag,uMsgID
		-- 修行塔里给绑定金
		if sceneType and sceneType == 14 then
			bFlag,uMsgID = MoneyManagerDB.AddMoneyByType(fun_info["FunName"],fun_info["MoneyAward"],playerId, 2, money,nil, event_type_tbl["修行塔给绑定金币"])
		else
			bFlag,uMsgID = MoneyManagerDB.AddMoney(fun_info["FunName"],fun_info["MoneyAward"],playerId, money,nil,event_type_tbl["副本奖励"])
		end
		if not bFlag then
			return false
		end
		moneyResult = {bFlag,uMsgID}
	end
	if itemTbl then
		local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
		local index
		for i=1, #(itemTbl) do
			index = i
			local item = itemTbl[i]
			if item then
				data["nType"]		= item[1]
				data["sName"] 		= item[2]
				if isFlag then
					data["nCount"]		= isFlag * item[3]
				else
					data["nCount"]		= item[3]
				end
				data["sceneName"]	= sceneName
				data["createType"]	= event_type_tbl["副本奖励"]
				local res = CharacterMediatorDB.AddItem(data)
				if IsNumber(res) then
					if res ~= 47 then --包裹空间不足 采用邮寄, 其他情况把 index 赋值为nil 不邮寄
						index = nil				
					end
					break
				else
					table.insert(itemResult,{item[1],item[2],item[3],res})
					index = nil
				end
			end
		end
		if index then
			local mailTbl = {}
			for i = index, #(itemTbl) do
				table.insert(mailTbl, itemTbl[i])
			end
			mailResult = SendItem(playerId, 0, 0, 0, mailTbl, MailTitle, MailContent,sceneName, isFlag)
			if not mailResult then
				return false
			end
		end
	end
	return true,{expResult, moneyResult, itemResult, soulResult, mailResult,fetch, soulValue}
end

local function SendItemOffLine(playerId)
	local chardb = RequireDbBox("CharacterMediatorDB")
	local data = {}
	data["char_id"] = playerId
	local result = chardb.GetPlayerStateData(data)
	local flag = 1
	for i = 0, result:GetRowNum() - 1 do
		if stateTbl[result:GetData(i,0)] then
			local res = stateTbl[result:GetData(i,0)] 
			return res[1], res[2]
		end
	end
	return flag
end

-- function RoleQuestDB:FbActionAward(data)
function RoleQuestDB.FbActionAward(data)
	local result = {}
	local MailTitle = data["MailTitle"]
	local MailContent = data["MailContent"]
	local sceneName = data["sceneName"]
	local sceneType = data["sceneType"]
	
	local AreaFbPointDB = RequireDbBox("AreaFbPointDB")
	for playerId, v in pairs(data) do
		if IsNumber(playerId) then
			if v.isInGame then --玩家在游戏中直接发奖励
				local resSucc, awardInfo = PlayerAward(playerId, v.exp , v.fetch, v.money, v.itemTbl, MailTitle, MailContent,sceneName,sceneType, v.isFlag, v.soul)
				if not resSucc then
					CancelTran()
					return false
				end
				result[playerId] = awardInfo
				if v.fightScore and v.fightScore ~= 0 then
					AreaFbPointDB.AddAreaFbPointByType(playerId, v.fightScore,8,sceneName,nil,event_type_tbl["竞技场副本加积分"])
				end
			else--不在游戏中,以邮寄的形式发奖励
				local flag, type = SendItemOffLine(playerId)
				if not type then
					result[playerId] = SendItem(playerId, v.exp, v.fetch, v.money, v.itemTbl, MailTitle, MailContent,sceneName, v.isFlag, v.soul)
				elseif type == "all" then
					result[playerId] = SendItem(playerId, flag * (v.exp or 0), flag * (v.fetch or 0), flag * (v.money or 0), v.itemTbl, MailTitle, MailContent,sceneName, flag * (v.isFlag or 0), flag * (v.soul or 0))
				elseif type == "exp" then
					result[playerId] = SendItem(playerId, flag * (v.exp or 0), v.fetch, v.money, v.itemTbl, MailTitle, MailContent,sceneName, v.isFlag, v.soul)
				elseif type == "item" then
					result[playerId] = SendItem(playerId, v.exp, v.fetch, v.money, v.itemTbl, MailTitle, MailContent,sceneName, flag * (v.isFlag or 0), v.soul)
				end
				
				if not result[playerId] then
					CancelTran()
					return false
				end
				if type and type ~= "item" and type ~= "exp" then
					if v.fightScore and v.fightScore ~= 0 then
						AreaFbPointDB.AddAreaFbPointByType(playerId, flag * v.fightScore,8,sceneName,nil,event_type_tbl["竞技场副本加积分"])
					end
				end
			end
		end
	end
	return true,result
end

function RoleQuestDB.MonsAttackTongAward(data)
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")	
  local cdepotMgr = RequireDbBox("GasCDepotMgrDB")
  local tong_box = RequireDbBox("GasTongBasicDB")
	local WarZoneDB = RequireDbBox("WarZoneDB")
	local nTongID = data["nTongID"]
	local nItemBigID = data["nItemBigID"]
	local sItemName = data["sItemName"]
	local nDepotIndex = data["nDepotIndex"]
	local sSceneName = data["sSceneName"]
	local nExperience = data["nExperience"]
	
	local leaderId = tong_box.GetTongLeaderId(nTongID)
	local nRelativeLine = WarZoneDB.GetTongRelativeLine(nTongID)
	local tongInfo = tong_box.GetTongInfo(nTongID)
	local nTongLevel = tongInfo.tong_level
	local nTongMemberNum = tongInfo.member_num
	local stationLevelModulus = TongStationLevelModulus_Server(nRelativeLine, "Modulus")
	local tongLevelModulus = TongLevelModulus_Server(nTongLevel, "Modulus")
	nExperience = nExperience * nTongMemberNum * stationLevelModulus * tongLevelModulus
	sItemName = sItemName .. nExperience
	
	local params= {}
	params.m_nType = nItemBigID
	params.m_sName = sItemName
	params.m_nBindingType = 0
	params.m_nCharID = leaderId
	params.m_sCreateType = event_type_tbl["怪物攻城奖励"]
	
	local itemID = g_RoomMgr.CreateItem(params)
	if not itemID then
		LogErr("怪物攻城奖励经验球创建失败","sBigID:"..nItemBigID.."sItemName:"..sItemName.."nCharID:"..leaderId)
	end
	local nRes = cdepotMgr.AutoAddItems(tong_box.GetTongDepotID(nTongID), nItemBigID, sItemName, {itemID}, nDepotIndex)
	if IsNumber(nRes) or not nRes then
		LogErr("怪物攻城奖励经验球发送仓库失败","nDepotIndex:"..nDepotIndex.."nItemBigID:"..nItemBigID.."sItemName:"..sItemName)
	end
	params.m_nType = data["nItem1BigID"]
	params.m_sName = data["sItem1Name"]
	
	local tbl_item_id = {}
	--创建四个荣誉奖章
	for i=1,4 do
		local itemID4 = g_RoomMgr.CreateItem(params)
		table.insert(tbl_item_id,itemID4)
	end
	local nRes = cdepotMgr.AutoAddItems(tong_box.GetTongDepotID(nTongID), params.m_nType, params.m_sName, tbl_item_id, nDepotIndex)

--暂时屏蔽宝石奖励
--	nItemBigID = data["nStoneBigID"]
--	params.m_nType = nItemBigID
--	
--	for j = 1, 4 do
--		sItemName = data["sStoneName" .. j]
--		params.m_sName = sItemName
--		for i = 1, 25 do
--			local nItemID = g_RoomMgr.CreateItem(params)
--			local nRes = cdepotMgr.AutoAddItems(tong_box.GetTongDepotID(nTongID), nItemBigID, sItemName,{nItemID}, nDepotIndex)
--		end
--	end
end

function RoleQuestDB.CountQuestLimitByName(sQuestName)
	local res = StmtContainer._CountQuestLimitByName:ExecStat(sQuestName)
	return res:GetData(0,0)
end

function RoleQuestDB.CountObjDropLimitByName(ObjName)
	local res = StmtContainer._CountObjDropLimitByName:ExecStat(ObjName)
	return res:GetData(0,0)
end

function RoleQuestDB.FirstQuestFromLeiMeng(data)
	local NoviceDirectDB = RequireDbBox("NoviceDirectDB")
	NoviceDirectDB.InsertPlayerFirstFinishInfo(data)
	return RoleQuestDB.AcceptQuest(data)
end

function RoleQuestDB.TempMemberExpAdd(data)
	local PlayerTempExp = data["PlayerTempExp"]
	--local ExpTbl = {}
	for id, exp in pairs(PlayerTempExp) do
		local param = {}
		param["char_id"] = id
		param["addExp"] = exp
		param["addExpType"] = event_type_tbl["杀怪获得经验"]
		local CurLevel, CurExp = RoleQuestDB.AddExp(param)
--		if CurLevel and CurExp then
--			ExpTbl[id] = {}
--			ExpTbl[id]["Level"] = CurLevel
--			ExpTbl[id]["Exp"] = CurExp
--			ExpTbl[id]["TeamAwardExp"] = exp * ExpCofe
--			ExpTbl[id]["AddExp"] = exp
--			ExpTbl[id]["uInspirationExp"] = 0
--		end
	end
	--return ExpTbl
end

local function GetLoopQuestState(CharId)
	local res = StmtContainer._GetAllQuestLoop:ExecStat(CharId)
	local tbl = {}
	if res and res:GetRowNum()>0 then
		for i=0, res:GetRowNum()-1 do
			local loopname = res:GetData(i,0)
			local loopnum = res:GetData(i,1)
			local iserror = true
			if g_LoopQuestMgr[loopname] and g_LoopQuestMgr[loopname][loopnum] then
				for n=1, #(g_LoopQuestMgr[loopname][loopnum]) do
					local questname = g_LoopQuestMgr[loopname][loopnum][n].QuestName
					if GetQuestState{CharId, questname} == QuestState.init then
						iserror = false
					end
				end
			end
			
			if iserror then
				StmtContainer._DeleteQuestLoop:ExecStat(CharId, loopname)
			else
				table.insert(tbl, {loopname, loopnum})
			end
		end 
	end
	return tbl
end

function RoleQuestDB.GetLoopQuestState(CharId)
	return GetLoopQuestState(CharId)
end

SetDbLocalFuncType(RoleQuestDB.AddQuestVarNum)
SetDbLocalFuncType(RoleQuestDB.AddQuestTheater)
SetDbLocalFuncType(RoleQuestDB.AddQuestVarNumForTeam)
SetDbLocalFuncType(RoleQuestDB.AddVarNumForTeamQuests)
SetDbLocalFuncType(RoleQuestDB.AddVarNumForLevelQuests)
SetDbLocalFuncType(RoleQuestDB.ShareQuest)
SetDbLocalFuncType(RoleQuestDB.AddNewQuest)
SetDbLocalFuncType(RoleQuestDB.AddNewQuestVar)
SetDbLocalFuncType(RoleQuestDB.AddQuest)
SetDbLocalFuncType(RoleQuestDB.GiveUpQuest)
SetDbLocalFuncType(RoleQuestDB.AcceptQuest)
SetDbLocalFuncType(RoleQuestDB.FinishQuest)
SetDbLocalFuncType(RoleQuestDB.GetAwardItemFromQuest)
SetDbLocalFuncType(RoleQuestDB.ItemRequestQuest)
SetDbLocalFuncType(RoleQuestDB.QuestFailure)
SetDbLocalFuncType(RoleQuestDB.QuestIsLimitTime)
SetDbLocalFuncType(RoleQuestDB.KillNpcAddQuestDropObj)
SetDbLocalFuncType(RoleQuestDB.FbActionAward)
SetDbLocalFuncType(RoleQuestDB.AddExp)
SetDbLocalFuncType(RoleQuestDB.CheckQuestVarNeedNum)
SetDbLocalFuncType(RoleQuestDB.CheckQuestItemNeedNum)
SetDbLocalFuncType(RoleQuestDB.CheckQuestSatisfy)
SetDbLocalFuncType(RoleQuestDB.LeaveSceneSetQuestFailure)
SetDbLocalFuncType(RoleQuestDB.MonsAttackTongAward)
SetDbLocalFuncType(RoleQuestDB.FirstQuestFromLeiMeng)
SetDbLocalFuncType(RoleQuestDB.TempMemberExpAdd)
SetDbLocalFuncType(RoleQuestDB.ClearQuest)
SetDbLocalFuncType(RoleQuestDB.DeleteAllQuestVar)
SetDbLocalFuncType(RoleQuestDB.GetQuestAcceptTime)
SetDbLocalFuncType(RoleQuestDB.GetQuestState)
SetDbLocalFuncType(RoleQuestDB.UpdateQuestState)
SetDbLocalFuncType(RoleQuestDB.UpdateQuestTime)
SetDbLocalFuncType(RoleQuestDB.InsertCharFinishQuestNum)
SetDbLocalFuncType(RoleQuestDB.SelectCharFinishQuestNum)
SetDbLocalFuncType(RoleQuestDB.UpdateCharFinishQuestNum)
SetDbLocalFuncType(RoleQuestDB.GetQuestExpMultiple)
SetDbLocalFuncType(RoleQuestDB.Is2CharHaveSameKillNpcQuest)
SetDbLocalFuncType(RoleQuestDB.SelectQuestVarList)
SetDbLocalFuncType(RoleQuestDB.SetQuestVarNum)
SetDbLocalFuncType(RoleQuestDB.DeleteQuestVar)
SetDbLocalFuncType(RoleQuestDB.GiveUpQuestSql)
SetDbLocalFuncType(RoleQuestDB.GetAllQuestVar)
SetDbLocalFuncType(RoleQuestDB.GetAllQuestSql)
SetDbLocalFuncType(RoleQuestDB.GetDoingQuestSql)
SetDbLocalFuncType(RoleQuestDB.GetLoopQuestState)
SetDbLocalFuncType(RoleQuestDB.GetAllQuestAwardItemSql)
SetDbLocalFuncType(RoleQuestDB.GetAllYbFinishQuest)
SetDbLocalFuncType(RoleQuestDB.GetNowQuestSql)
SetDbLocalFuncType(RoleQuestDB.CanAcceptQuest)
SetDbLocalFuncType(RoleQuestDB.CheckNeedItemNum)
SetDbLocalFuncType(RoleQuestDB.IntoTrapAddItem)
SetDbLocalFuncType(RoleQuestDB.SingleGridItem)
SetDbLocalFuncType(RoleQuestDB.GiveQuestAward)
SetDbLocalFuncType(RoleQuestDB.GutQuestFailure)
SetDbLocalFuncType(RoleQuestDB.AllPlayerQuestFail)
SetDbLocalFuncType(RoleQuestDB.DeleteQuest)
SetDbLocalFuncType(RoleQuestDB.DecreQuestItemVar)
SetDbLocalFuncType(RoleQuestDB.DeleteItem)
SetDbLocalFuncType(RoleQuestDB.DeleteItemByPos)
SetDbLocalFuncType(RoleQuestDB.ConsumeItem)
SetDbLocalFuncType(RoleQuestDB.UseItemAddExp)
SetDbLocalFuncType(RoleQuestDB.CountQuestLimitByName)
SetDbLocalFuncType(RoleQuestDB.CountObjDropLimitByName)
return RoleQuestDB


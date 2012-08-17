cfg_load "fb_game/JoinCountLimit_Server"
cfg_load "fb_game/FbActionDirect_Common"
gac_gas_require "activity/scene/AreaFbSceneMgr"
cfg_load "fb_game/MatchGame_Common"
cfg_load "item/MatchGameItem_Common"


local os = os
local loadstring = loadstring
local MatchGameItem_Common = MatchGameItem_Common
local JoinCountLimit_Server = JoinCountLimit_Server
local FbActionDirect_Common = FbActionDirect_Common
local AreaFb_Common = AreaFb_Common
local g_AreaFbSceneMgr = g_AreaFbSceneMgr
local GetCfgTransformValue = GetCfgTransformValue
local MatchGame_Common = MatchGame_Common
local StmtContainer = class()
local event_type_tbl = event_type_tbl

local StmtDef=
{
	"InsertRecord",
	[[
	insert into 
		tbl_activity_count(cs_uId, ac_sActivityName, ac_dtTime) 
		values(?,?,now())
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"InsertHistoryRecord",
	[[
	insert into 
		tbl_activity_history_count(cs_uId, ac_sActivityName, ac_HistoryTimes, ac_SuccessfullTimes) 
		values(?,?,?,?)
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"AddHistoryRecordCount",
	[[
	update 
		tbl_activity_history_count
	set
		ac_HistoryTimes = ac_HistoryTimes + 1
	where
		cs_uId = ? and ac_sActivityName = ?
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"AddSuccessRecordCount",
	[[
	update 
		tbl_activity_history_count
	set
		ac_SuccessfullTimes = ac_SuccessfullTimes + 1
	where
		cs_uId = ? and ac_sActivityName = ?
	]]
}
DefineSql(StmtDef,StmtContainer)


local StmtDef=
{
	"GetCurCycJoinCount",
	[[
	select 
		count(*) 
	from 
		tbl_activity_count
	where
		cs_uId = ? and ac_sActivityName = ? and ac_dtTime >= from_unixtime(?)
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"GetCurCDJoinCount",
	[[
	select 
		count(*) 
	from 
		tbl_activity_count
	where
		cs_uId = ? and ac_sActivityName = ?
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"ClearCurCycJoinCount",
	[[
	delete from
		tbl_activity_count
	where
		cs_uId = ? and ac_sActivityName = ?
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"ClearHistoryJoinCount",
	[[
	delete from
		tbl_activity_history_count
	where
		cs_uId = ? and ac_sActivityName = ?
	]]
}
DefineSql(StmtDef,StmtContainer)


local StmtDef=
{
	"ClearJoinCount",
	[[
	delete from
		tbl_activity_count
	where
		cs_uId = ?
	]]
}
DefineSql(StmtDef,StmtContainer)


local StmtDef=
{
	"GetHistoryJoinCount",
	[[
	select 
		ac_HistoryTimes 
	from 
		tbl_activity_history_count
	where
		cs_uId = ? and ac_sActivityName = ?
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"GetSuccessJoinCount",
	[[
	select 
		ac_SuccessfullTimes 
	from 
		tbl_activity_history_count
	where
		cs_uId = ? and ac_sActivityName = ?
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"ClearPreteritData",
	[[
	delete from  
		tbl_activity_count 
	where 
		ac_dtTime < from_unixtime(?) and ac_sActivityName = ?
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"ClearPreteritDataById",
	[[
	delete from  
		tbl_activity_count 
	where 
		ac_dtTime < from_unixtime(?) and ac_sActivityName = ? and cs_uId = ?
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef = 
{
		"GetCharacterCreateData",
		[[
			select unix_timestamp(cs_dtCreateDate) from tbl_char_static where cs_uId = ?
		]]
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = 
{
		"GetStoreActionData",
		[[
			select unix_timestamp(asd_dtTime), asd_uStoreTimes from tbl_activity_store_data where cs_uId = ? and asd_sActivityName = ?
		]]
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = 
{
		"InsertStoreActionData",
		[[
			insert into 
			tbl_activity_store_data(cs_uId, asd_sActivityName, asd_dtTime, asd_uStoreTimes) 
			values(?,?,now(),?)
		]]
}
DefineSql(StmtDef, StmtContainer)
	
local StmtDef = 
{
		"UpDataStoreActionData",
		[[
			update
				tbl_activity_store_data
			set
				asd_uStoreTimes = ?, asd_dtTime = now()
			where
				cs_uId = ? and asd_sActivityName = ?
		]]
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = 
{
		"ReStoreStoreActionData",
		[[
			update 
				tbl_activity_store_data as a
			set
				a.asd_uStoreTimes = a.asd_uStoreTimes + (select count(*) from tbl_activity_count where cs_uId = a.cs_uId and a.asd_sActivityName = ac_sActivityName) 
			where
				cs_uId = ?
		]]
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = 
{
		"_GetActivityCount",
		"select aic_uInTimes from tbl_activity_in_count as aic where aic.cs_uId = ? and aic.aic_sActivityName = ?"
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = 
{
		"_GetAllActivityCount",
		"select aic.aic_sActivityName, aic_uInTimes from tbl_activity_in_count as aic where aic.cs_uId = ?"
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = 
{
		"_InsertActivityCount",
		[[
			insert into 
				tbl_activity_in_count(cs_uId, aic_sActivityName, aic_uInTimes)values(?,?,1)
		]]
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = 
{
		"_UpdateActivityCount",
		[[
			update 
				tbl_activity_in_count as aic
			set
				aic.aic_uInTimes = aic.aic_uInTimes + 1
			where
				aic.cs_uId = ? and aic.aic_sActivityName = ?
		]]
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = 
{
		"_GetDayActivityCountTime",
		[[
			select  
				count(*)
			from 
				tbl_activity_count
			where
				cs_uId = ? and ac_sActivityName = ? and date(ac_dtTime) = date(now())
		]]
}
DefineSql(StmtDef, StmtContainer)


local ActivityCountDbBox = CreateDbBox(...)

------------------------------------------------------------------------------

local JoinCountLimitTbl = {}   --存放转换好的JoinCountLimit_Server 配置表信息

for _, name in pairs(JoinCountLimit_Server:GetKeys()) do
	JoinCountLimitTbl[name] = {}
	JoinCountLimitTbl[name].CriticalTime = GetCfgTransformValue(true, "JoinCountLimit_Server", name, "CriticalTime")
	if JoinCountLimitTbl[name].CriticalTime.wday then
		if JoinCountLimitTbl[name].CriticalTime.wday == 7 then
			JoinCountLimitTbl[name].CriticalTime.wday = 1
		else
			JoinCountLimitTbl[name].CriticalTime.wday = JoinCountLimitTbl[name].CriticalTime.wday + 1
		end
	end
end

local function GetCriticalTime(Date, MaxTimes)
	local ActionCriticalTime = JoinCountLimitTbl[MaxTimes].CriticalTime
	local criticalDate = {}
	
	criticalDate.year = Date.year
	criticalDate.month = Date.month
	criticalDate.day = Date.day
	criticalDate.hour = ActionCriticalTime.hour
	criticalDate.min = ActionCriticalTime.min	
	
	local startTime = os.time(criticalDate)
	if ActionCriticalTime.hour > Date.hour 
		or(ActionCriticalTime.hour == Date.hour and ActionCriticalTime.min > Date.min) then
		startTime = startTime - 24 * 60 * 60
	end
	return startTime
end

-- 角色创建时间是否是在本周期内
local function GetCharacterLevelUpTime(name, charId)
	-- 角色升级到相应等级的时间
	local Level
	for _, ActionName in pairs(FbActionDirect_Common:GetKeys()) do
		local MaxTimes = FbActionDirect_Common(ActionName, "MaxTimes")
		if MaxTimes == name then
			Level = FbActionDirect_Common(ActionName, "MinLevel")
			break
		end
	end
	
	local GasRoleLevelUpAttentionDB = RequireDbBox("GasRoleLevelUpAttentionDB")
	local CharaLevelUpTime = GasRoleLevelUpAttentionDB.GetPlayerLevelUpTime(charId, Level)
	
	-- 如果没有找到 CharaLevelUpTime ，则为今天
	if not CharaLevelUpTime then
		CharaLevelUpTime = os.time() --ActivityCountDbBox.GetActivityCriticalTime(name)
	end
	return CharaLevelUpTime
end

function ActivityCountDbBox.InsertStoreActionData(charId, maxTimes, StoreTimes)
	StmtContainer.InsertStoreActionData:ExecStat(charId, maxTimes, StoreTimes)
end

function ActivityCountDbBox.UpDataStoreActionData(StoreNum, charId, maxTimes)
	StmtContainer.UpDataStoreActionData:ExecStat(StoreNum, charId, maxTimes)
end

-- 返回  日期，当天的累计次数
function ActivityCountDbBox.GetStoreActionData(charId, maxTimes)
	local result = StmtContainer.GetStoreActionData:ExecStat(charId, maxTimes)
	
	-- 今天的临界时间
	local TodayCriticalTime = GetCriticalTime(os.date("*t"), maxTimes)

	local StoreNum
	-- 如果没有记录,计算升级那天到今天的天数
	if result:GetRowNum() == 0 then
		result:Release()
		
		-- 升级的时间
		local LevelUpTime = GetCharacterLevelUpTime(maxTimes, charId)
		-- 升级那天的临界时间
		local LevelUpCriticalTime = GetCriticalTime(os.date("*t", LevelUpTime), maxTimes)
		
		local DaysNum = (TodayCriticalTime - LevelUpCriticalTime) / (24*60*60) + 1
		
		StoreNum = JoinCountLimit_Server(maxTimes, "Count") * DaysNum
		if StoreNum > 7 then
			StoreNum = 7
		end
		-- 插入记录
		ActivityCountDbBox.InsertStoreActionData(charId, maxTimes, StoreNum)
	else
		-- 数据库里存的时间的临界时间
		local StoreTime = GetCriticalTime(os.date("*t", result:GetData(0,0)), maxTimes) 
		-- 相隔了几天
		local DaysNum = (TodayCriticalTime - StoreTime) / (24*60*60)
		
		-- 原记录+累加的次数
		StoreNum = result:GetData(0,1) + JoinCountLimit_Server(maxTimes, "Count") * DaysNum 
		result:Release()	
		
		if StoreNum > 7 then
			StoreNum = 7
		end
		-- 如果当前时间和表里存的不是一天，更新记录
		if DaysNum ~= 0 then
			ActivityCountDbBox.UpDataStoreActionData(StoreNum, charId, maxTimes)
		end
	end
	
	return StoreNum
end

function ActivityCountDbBox.GetActivityCDTime(name, charId)
	local date = os.time()
	local ResetMode = g_AreaFbSceneMgr[name].ResetMode
	local CoolDownDay = ResetMode[2]
--	print("CollDownDay -- "..CoolDownDay)
	date = date - CoolDownDay * 24 * 60 * 60
	
	StmtContainer.ClearPreteritDataById:ExecStat(date, name, charId)
	return date
end

function ActivityCountDbBox.GetCharacterCreateData(charId)
	local result = StmtContainer.GetCharacterCreateData:ExecStat(charId)
	local data = result:GetData(0,0)
	result:Release()
	return data
end

--获得活动次数限制的临界时间点
function ActivityCountDbBox.GetActivityCriticalTime(name)
	if JoinCountLimitTbl[name].StartTime then
		if os.time() >= JoinCountLimitTbl[name].EndTime then
			JoinCountLimitTbl[name].StartTime = JoinCountLimitTbl[name].EndTime
			if JoinCountLimit_Server(name, "Cycle") == "day" then
				JoinCountLimitTbl[name].EndTime = JoinCountLimitTbl[name].EndTime + 24 * 60 * 60
			elseif JoinCountLimit_Server(name, "Cycle") == "week"
			or JoinCountLimit_Server(name, "Cycle") == "store" then
				JoinCountLimitTbl[name].EndTime = JoinCountLimitTbl[name].EndTime + 7 * 24 * 60 * 60
			end
			ActivityCountDbBox.ClearPreteritData(name, JoinCountLimitTbl[name].StartTime)
		end
		return JoinCountLimitTbl[name].StartTime
	end

	local nowDate = os.date("*t")
	local criticalTime = JoinCountLimitTbl[name].CriticalTime
	
	local criticalDate = {}
	criticalDate.year = nowDate.year
	criticalDate.month = nowDate.month
	criticalDate.day = nowDate.day
	criticalDate.hour = criticalTime.hour
	criticalDate.min = criticalTime.min
	local startTime = os.time(criticalDate)
	local endTime
	
	if JoinCountLimit_Server(name, "Cycle") == "day" 
		or JoinCountLimit_Server(name, "Cycle") == "store" then
		if criticalTime.hour > nowDate.hour 
			or(criticalTime.hour == nowDate.hour and criticalTime.min > nowDate.min) then
			startTime = startTime - 24 * 60 * 60
		end
		endTime = startTime + 24 * 60 * 60
	elseif JoinCountLimit_Server(name, "Cycle") == "week" then
		if criticalTime.wday < nowDate.wday then
			startTime = startTime - (nowDate.wday - criticalTime.wday) * 24 * 60 * 60
		elseif criticalTime.wday > nowDate.wday then
			startTime = startTime - (nowDate.wday + 7 - criticalTime.wday) * 24 * 60 * 60
		else -- if criticalTime.wday == nowDate.wday then
			if criticalTime.hour > nowDate.hour 
				or(criticalTime.hour == nowDate.hour and criticalTime.min > nowDate.min) then
				startTime = startTime - 7 * 24 * 60 * 60
			end
		end
		endTime = startTime + 7 * 24 * 60 * 60
	end
	
	JoinCountLimitTbl[name].StartTime  = startTime
	JoinCountLimitTbl[name].EndTime  = endTime
	ActivityCountDbBox.ClearPreteritData(name, JoinCountLimitTbl[name].StartTime)
	return JoinCountLimitTbl[name].StartTime
end

function ActivityCountDbBox.GetAllActivityCount(data)
	local PlayerID = data["char_id"]
	local result = {}
	
	local JoinActionDB = RequireDbBox("JoinActionDB")
	JoinActionDB.SetActionPanelWarnValue(PlayerID)
	
	--for ActionName, v in pairs(FbActionDirect_Common) do
	for _, ActionName in pairs(FbActionDirect_Common:GetKeys()) do
		-- 等级够了才查询
		local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
		local uCharLevel = CharacterMediatorDB.GetPlayerLevel(PlayerID)
		local ActionLevel = FbActionDirect_Common(ActionName, "MinLevel")
		
		if uCharLevel >= ActionLevel then
			local MaxTimes = FbActionDirect_Common(ActionName, "MaxTimes")
			if MaxTimes ~= nil and MaxTimes ~= "" and MaxTimes ~= "不限制参加次数" then
				local criticalTime
				if JoinCountLimit_Server(MaxTimes, "Cycle") == "store" then
					-- 获取今天参加的次数
					local nowDate = os.date("*t")
					-- 今天的临界时间
					criticalTime = GetCriticalTime(nowDate, MaxTimes)
					result[ActionName] ={}
					result[ActionName][1] = ActivityCountDbBox.GetActivityTimes(PlayerID, MaxTimes, criticalTime)
					
					-- 获得 当前可以参加的次数
					local StoreNum = ActivityCountDbBox.GetStoreActionData(PlayerID, MaxTimes)
					-- 今天总共可以参加的次数 = 当前可以参加的次数+今天已经参加的次数
					result[ActionName][2] = StoreNum + result[ActionName][1]
				else
					criticalTime = ActivityCountDbBox.GetActivityCriticalTime(MaxTimes)
					result[ActionName] = {}
					result[ActionName][1] = ActivityCountDbBox.GetActivityTimes(PlayerID, MaxTimes, criticalTime)
				end
			else
				result[ActionName] = {}
				result[ActionName][1] = -1
			end
		end
	end
	return result
end

function ActivityCountDbBox.GetAllDungeonCount(data)
	local PlayerID = data["char_id"]
	local result = {}
	--for ActionName, v in pairs(FbActionDirect_Common) do
	for _, FbName in pairs(AreaFb_Common:GetKeys()) do
		--local MaxTimes = AreaFb_Common(ActionName, "LimitType")
		
		local ActionName = AreaFb_Common(FbName, "ActionName")
 		local MaxTimes = FbActionDirect_Common(ActionName, "MaxTimes")
  
		if MaxTimes ~= nil and MaxTimes ~= "" and MaxTimes ~= "不限制参加次数" then
			local criticalTime = ActivityCountDbBox.GetActivityCriticalTime(MaxTimes)
			result[FbName] = ActivityCountDbBox.GetActivityTimes(PlayerID, MaxTimes, criticalTime)
		else
			result[FbName] = -1
		end
	end
	return result
end

function ActivityCountDbBox.GetOneActivityCount(data)
	local PlayerID = data["char_id"]
	local ActionName = data["ActionName"]
	local activityName = data["activityName"]
	local count 
	local MaxTimes = FbActionDirect_Common(ActionName, "MaxTimes")
	if MaxTimes ~= nil and MaxTimes ~= "" and MaxTimes ~= "不限制参加次数" then
		local criticalTime = ActivityCountDbBox.GetActivityCriticalTime(MaxTimes)
		count = ActivityCountDbBox.GetActivityTimes(PlayerID, MaxTimes, criticalTime)
		local maxCount = JoinCountLimit_Server(MaxTimes, "Count")
		local flag, sec, resCount = ActivityCountDbBox.CheckActivityCount(PlayerID, activityName, criticalTime)
		return count, resCount
	else
		count = -1
		return count, 0
	end
end

function ActivityCountDbBox.GetActivityTimes(playerId, activityName, criticalTime)
	local result = StmtContainer.GetCurCycJoinCount:ExecSql("n", playerId, activityName, criticalTime)
	local count = result:GetData(0,0)
	result:Release()
	--print(">>>>>>>>>>>>>>>>>>玩家 " .. playerId .. " 当前周期 " .. activityName .. " 的参加次数为: " ..  count .. "  分界时间 .. " .. criticalTime)
	return count
end

-- 获取玩家参加activityName活动成功的次数
function ActivityCountDbBox.GetActivitySuccessTimes(playerId, activityName)
	local result = StmtContainer.GetSuccessJoinCount:ExecSql("n", playerId, activityName)
	local count = result:GetData(0,0)
	result:Release()
	--print(">>>>>>>>>>>>>>>>>>玩家 " .. playerId .. " " .. activityName .. " 的成功次数为: " ..  count)
	return count
end

-- 获取玩家参加activityName活动的总次数
function ActivityCountDbBox.GetActivityHistoryTimes(playerId, activityName)
	local result = StmtContainer.GetHistoryJoinCount:ExecSql("n", playerId, activityName)
	if result:GetRowNum() == 0 then
		ActivityCountDbBox.InsertActivityHistoryCount(playerId, activityName, 0, 0)
		result:Release()
		return 0
	end
	
	local count = result:GetData(0,0)
	result:Release()
	--print(">>>>>>>>>>>>>>>>>>玩家 " .. playerId .. " " .. activityName .. " 的总次数为: " ..  count)
	return count
end

function ActivityCountDbBox.CheckActivityCount(playerId, activityName, criticalTime, maxCount)
	local result = StmtContainer.GetCurCycJoinCount:ExecSql("n", playerId, activityName, criticalTime)
	local count = result:GetData(0,0)
	result:Release()
	--print(">>>>>>>>>>>>>>>>>>玩家 " .. playerId .. " 当前周期 " .. activityName .. " 的参加次数为: " ..  count .. "  分界时间 .. " .. criticalTime)
	--加一个参数,返回现在的次数
	if maxCount then
		return count < maxCount, count
	else
		local ActionMaxCount
		-- 特定的活动可以累计次数
		if JoinCountLimit_Server(activityName, "Cycle") == "store" then
			-- 今天总共可以参加的次数 = 当前累积可用的次数 + 已经参加过的次数
			local StoreNum = ActivityCountDbBox.GetStoreActionData(playerId, activityName) 
			ActionMaxCount = StoreNum + count
		else
			ActionMaxCount = JoinCountLimit_Server(activityName, "Count")
		end
		return count < ActionMaxCount, count, ActionMaxCount - count
	end
end

function ActivityCountDbBox.CheckCDActivityCount(playerId, activityName, criticalTime)
	local result = StmtContainer.GetCurCycJoinCount:ExecSql("n", playerId, activityName, criticalTime)
	local count = result:GetData(0,0)
	result:Release()
	--print(">>>>>>>>>>>>>>>>>>玩家 " .. playerId .. " 当前周期 " .. activityName .. " 的参加次数为: " ..  count)
	return count < 1
end

function ActivityCountDbBox.GetActivityCount(activityName, maxCount)
	if maxCount then
		return maxCount
	else
		return JoinCountLimit_Server(activityName, "Count")
	end
end

function ActivityCountDbBox.CheckMultiPlayerActivityCount(data)
	local playerTbl = data["PlayerTbl"] 
	local activityName = data["ActivityName"]
	local criticalTime = ActivityCountDbBox.GetActivityCriticalTime(activityName)
	for playerId in pairs(playerTbl) do
		if not ActivityCountDbBox.CheckActivityCount(playerId, activityName, criticalTime) then
			return false, playerId
		end
	end
	return true
end

function ActivityCountDbBox.CheckActionLimitCount(data)
	local flag, sec, resCount = ActivityCountDbBox.CheckSinglePlayerActivityCount(data)
	if resCount == 1 then
		local param = {["CharId"] = data["PlayerId"], ["SceneId"] = 0}
		local JoinActionDB = RequireDbBox("JoinActionDB")
		local ActionTbl = JoinActionDB.GetAllActionName(param)
		for i, v in pairs(ActionTbl) do
			if MatchGame_Common(v,"JoinLimitType") == data["ActivityName"] then
				return false
			end
		end
	end
	return flag
end

function ActivityCountDbBox.CheckSinglePlayerActivityCount(data)
	local playerId = data["PlayerId"]
	local activityName = data["ActivityName"]
	local criticalTime = ActivityCountDbBox.GetActivityCriticalTime(activityName)
	local maxCount = data["MaxCount"]
	return ActivityCountDbBox.CheckActivityCount(playerId, activityName, criticalTime, maxCount)
end

function ActivityCountDbBox.CheckCDSinglePlayerActivityCount(data)
	local playerId = data["PlayerId"]
	local activityName = data["ActivityName"]
	local criticalTime = ActivityCountDbBox.GetActivityCDTime(activityName, playerId) --ActivityCountDbBox.GetActivityCriticalTime(activityName)
--	local maxCount = data["MaxCount"]
	return ActivityCountDbBox.CheckCDActivityCount(playerId, activityName, criticalTime)
end

local function GetItemAndCount(playerId,actionName , ItemName, activityName)
	local g_RoomMgr		= RequireDbBox("GasRoomMgrDB")	
	local data = {}
	data["char_id"] = playerId
	data["ActionName"] = actionName
	data["activityName"] = activityName
	if not ItemName then
		return 
	end
	local dataTbl = loadstring(" return { " .. ItemName .. " }")()
	local tbl = {}
	for _, itemName in pairs(dataTbl) do
		local uHaveCount = g_RoomMgr.GetItemCount(playerId, 46, itemName)
		local count, resCount = ActivityCountDbBox.GetOneActivityCount(data)
		local times = MatchGameItem_Common(itemName, "Times")
		local useTime = MatchGameItem_Common(itemName, "UseTime")
		if count then
			if times > 0 and useTime == 0 then
				if uHaveCount > 0 and resCount >=  times then
					table.insert(tbl, {itemName, uHaveCount})
				end
			else 
				if uHaveCount > 0 then
					table.insert(tbl, {itemName,uHaveCount})
				end
			end
		end
	end
	return tbl
end


function ActivityCountDbBox.AddActivityCount(data)
	local uCharId = data["PlayerId"]
	local activityName = data["ActivityName"]
	local SceneName = data["SceneName"]
	local ItemName = data["ItemName"]
	local ActionName = data["ActionName"]
	if not ActionName then
		for _, Name in pairs(FbActionDirect_Common:GetKeys()) do
			local MaxTimes = FbActionDirect_Common(Name, "MaxTimes")
			if MaxTimes == activityName then
				ActionName = Name
				break
			end
		end
	end
	
	local res = GetItemAndCount(uCharId, ActionName, ItemName, activityName)
	--print("添加 " .. activityName .. " 的记录. ")
	StmtContainer.InsertRecord:ExecSql("", uCharId, activityName)
	if ActionName == "异界守护塔队伍" or ActionName == "抱小猪选拔赛单人版" then
		AddActionHistoryCount(uCharId, ActionName)
	end
	local cueOnce =	FbActionDirect_Common(ActionName, "CueOnce")
	if cueOnce and cueOnce == 1 then
		AddActionHistoryCount(uCharId, ActionName)
	end
	
	-- 如果是 store 类型的活动
	if JoinCountLimit_Server(activityName, "Cycle") == "store" then
		-- 
		local result = StmtContainer.GetStoreActionData:ExecStat(uCharId, activityName)
		if result:GetRowNum() == 0 then
			result:Release()
		else
			local StoreNum = result:GetData(0,1) - 1
			ActivityCountDbBox.UpDataStoreActionData(StoreNum, uCharId, activityName)
			result:Release()
		end
	end

	-- 通知活动提示面板更新次数
	if ActionName then
		Db2CallBack("UpdataActionNum", uCharId, ActionName, 1)
	end
	return res
end

function ActivityCountDbBox.AddActionHistoryCount(uCharId, ActionName)
	local res = StmtContainer.GetHistoryJoinCount:ExecStat(uCharId, ActionName)
	if res and res:GetRowNum()>0 then
		StmtContainer.AddHistoryRecordCount:ExecStat(uCharId, ActionName)
	else
		StmtContainer.InsertHistoryRecord:ExecStat(uCharId, ActionName, 1, 0)
	end
	res:Release()
end

function ActivityCountDbBox.AddXiuXingTaCount(data)
	ActivityCountDbBox.AddActivityCount(data)
	local uCharId = data["PlayerId"]
	local activityName = data["ActivityName"]
	StmtContainer.AddHistoryRecordCount:ExecStat(uCharId, activityName)
	
	data["charId"] = uCharId
	ActivityCountDbBox.RecordActionTimes(data)
end

function ActivityCountDbBox.AddAreaFbExtraCount(uCharId, ExtraLimitType)
	StmtContainer.InsertRecord:ExecSql("", uCharId, ExtraLimitType)
end

function ActivityCountDbBox.AddSuccessRecordCount(data)
	local uCharId = data["PlayerId"]
	local activityName = data["ActivityName"]
	StmtContainer.AddSuccessRecordCount:ExecSql("", uCharId, activityName)
end

-- 初始化 
function ActivityCountDbBox.InsertActivityHistoryCount(uCharId, activityName, AllCount, SuccessCount)
	if uCharId and activityName and SuccessCount then
		StmtContainer.InsertHistoryRecord:ExecStat(uCharId, activityName, AllCount, SuccessCount)
		return g_DbChannelMgr:LastAffectedRowNum() > 0
	end
end

function ActivityCountDbBox.ClearPreteritData(activityName, criticalTime)
	--print("删除 " .. activityName .. " 的记录. ")
	StmtContainer.ClearPreteritData:ExecSql("", criticalTime, activityName)
end

function ActivityCountDbBox.CheckAreaFbCount(charId, limitName)
	if limitName == "" then
		return true
	else
		local data = {}
		data["PlayerId"] = charId
		data["ActivityName"] = limitName
		local isCan, count = ActivityCountDbBox.CheckSinglePlayerActivityCount(data)
		if isCan then
			return true
		else
			return false, JoinCountLimit_Server(limitName, "Count")
		end
	end
end

function ActivityCountDbBox.CheckCoolDownAreaFbCount(charId, limitName)
	if limitName == "" then
		return true
	else
		local data = {}
		data["PlayerId"] = charId
		data["ActivityName"] = limitName
		local isCan = ActivityCountDbBox.CheckCDSinglePlayerActivityCount(data)
		return isCan
	end
end

function ActivityCountDbBox.CheckFbActionCount(charId, actionName, MaxCount)
	local limitName = FbActionDirect_Common(actionName, "MaxTimes")
	if limitName == "" then
		return true
	else
		local data = {}
		data["PlayerId"] = charId
		data["ActivityName"] = limitName
		data["MaxCount"] = MaxCount
		local isCan, count = ActivityCountDbBox.CheckSinglePlayerActivityCount(data)
		if isCan then
			return true
		else
			return false,MaxCount or JoinCountLimit_Server(limitName, "Count")
		end
	end
end

function ActivityCountDbBox.ClearCurCycJoinCount(CharId, ActivityName)
	StmtContainer.ClearCurCycJoinCount:ExecSql("", CharId, ActivityName)
	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
		return true
	end
end

function ActivityCountDbBox.ClearHistoryJoinCount(CharId, ActivityName)
	StmtContainer.ClearHistoryJoinCount:ExecSql("", CharId, ActivityName)
	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
		return true
	end
end

function ActivityCountDbBox.ClearActionJoinCount(data)
	local CharId = data.CharId
	local ActivityName = data.ActivityName
	ActivityCountDbBox.ClearCurCycJoinCount(CharId, ActivityName)
end

function ActivityCountDbBox.ClearCurCycAndHistoryCount(data)
	local CharId = data.CharId
	local ActivityName = data.ActivityName
	ActivityCountDbBox.ClearCurCycJoinCount(CharId, ActivityName)
	ActivityCountDbBox.ClearHistoryJoinCount(CharId, ActivityName)
end

function ActivityCountDbBox.ClearJoinCount(data)
	-- store 类型的活动
	StmtContainer.ReStoreStoreActionData:ExecStat(data["charId"])
	
	StmtContainer.ClearJoinCount:ExecStat(data["charId"])
	
	local JoinActionDB = RequireDbBox("JoinActionDB")
	JoinActionDB.ClearWarnValue(data["charId"])
end

function ActivityCountDbBox.AddTeamAreaFbCount(data)
	local PlayerTbl = data["PlayerTbl"]
	for _, PlayerId in pairs(PlayerTbl) do
		data["PlayerId"] = PlayerId
		ActivityCountDbBox.AddAreaFbCount(data)
	end
end

function ActivityCountDbBox.AddAreaFbCount(data)
	ActivityCountDbBox.AddActivityCount(data)
	local ExtraLimitType = data["ExtraLimitType"]
	if ExtraLimitType then
		ActivityCountDbBox.AddAreaFbExtraCount(data["PlayerId"], data["ExtraLimitType"])
	end
end

function ActivityCountDbBox.UpdateActionInfoAndDel(data)   --加次数,删物品,加buff
	local charId = data["charId"] 
	local actionName =  data["actionName"] 
	local itemName = data["itemName"]
	local times = data["times"]
	local activityName = data["activityName"]
	local g_RoomMgr		= RequireDbBox("GasRoomMgrDB")	
	local uHaveCount = g_RoomMgr.GetItemCount(charId, 46,itemName)
	
	local res = g_RoomMgr.DelItem(charId, 46, itemName, 1, nil, event_type_tbl["物品使用"])
	if IsNumber(res) then
		return {false,res}
	end
	if times > 2 then
		for i = 0, times - 2 do
			StmtContainer.InsertRecord:ExecSql("", charId, activityName)
		end
		if not (g_DbChannelMgr:LastAffectedRowNum() > 0) then
			CancelTran()
			return {false}
		end
		
		local result = StmtContainer.GetStoreActionData:ExecStat(charId, activityName)
		if result:GetRowNum() == 0 then
			result:Release()
		else
			local StoreNum = result:GetData(0,1) - times + 1
			ActivityCountDbBox.UpDataStoreActionData(StoreNum, charId, activityName)
			result:Release()
		end
		Db2CallBack("UpdataActionNum", charId, actionName, times - 1)
	end
	return {true,res}
end

--查询参加的总次数
function ActivityCountDbBox.GetActionAllTimes(data)
	local charId = data["charId"]
	local SceneName = data["SceneName"]
	local res = StmtContainer._GetActivityCount:ExecStat(charId, SceneName)
	if res:GetRowNum() > 0 then
		return res:GetData(0,0)
	else
		return 0
	end
end

--查询所有有记录活动的参加总次数
function ActivityCountDbBox.GetAllActionAllTimes(data)
	local charId = data["char_id"]
	return StmtContainer._GetAllActivityCount:ExecStat(charId)
end

--记录活动参加的总次数
function ActivityCountDbBox.RecordActionTimes(data)
	local charId = data["charId"]
	local SceneName = data["SceneName"]
	local res = StmtContainer._GetActivityCount:ExecStat(charId, SceneName)
	
	if res:GetRowNum() <= 0 then
		StmtContainer._InsertActivityCount:ExecStat(charId, SceneName)
	else
		StmtContainer._UpdateActivityCount:ExecStat(charId, SceneName)
	end
end

function ActivityCountDbBox.GetGuideInfo(data)
	return {ActivityCountDbBox.GetAllActivityCount(data), ActivityCountDbBox.GetAllActionAllTimes(data)}
end

function ActivityCountDbBox.GetDayActivityCount(charTbl, maxTimes)
	local tbl = {}
	local num = charTbl:GetRowNum()
	for i = 0 , num - 1 do
		local charId = charTbl:GetData(i,0)
		local SceneMgrDB = RequireDbBox("SceneMgrDB")
		if SceneMgrDB.IsCharInMainScene(charId) then
			local res = StmtContainer._GetDayActivityCountTime:ExecStat(charId, maxTimes)
			if res:GetData(0,0) == 0 then
				table.insert(tbl, charTbl:GetData(i,0))
			end
		end
	end
	return tbl
end

function ActivityCountDbBox.GetCanJoinAction(data)
	local charId = data["char_id"]
	local actionInfo = data["actionInfo"]
	local canJoinActionTbl = {}
	local SceneMgrDB = RequireDbBox("SceneMgrDB")
	for _, actionName in pairs(actionInfo) do
		local isCan, count = ActivityCountDbBox.CheckFbActionCount(charId, actionName)
		local flag = SceneMgrDB.IsCharInMainScene(charId)
		if isCan and flag then
			table.insert(canJoinActionTbl, actionName)
		end
	end
	return canJoinActionTbl
end


SetDbLocalFuncType(ActivityCountDbBox.GetAllActivityCount)
SetDbLocalFuncType(ActivityCountDbBox.GetActivityCount)
SetDbLocalFuncType(ActivityCountDbBox.GetAllDungeonCount)
SetDbLocalFuncType(ActivityCountDbBox.GetActivitySuccessTimes)
SetDbLocalFuncType(ActivityCountDbBox.GetActivityHistoryTimes)
SetDbLocalFuncType(ActivityCountDbBox.AddActivityCount)
SetDbLocalFuncType(ActivityCountDbBox.AddXiuXingTaCount)
SetDbLocalFuncType(ActivityCountDbBox.AddActionHistoryCount)
SetDbLocalFuncType(ActivityCountDbBox.CheckSinglePlayerActivityCount)
SetDbLocalFuncType(ActivityCountDbBox.ClearJoinCount)
SetDbLocalFuncType(ActivityCountDbBox.AddTeamAreaFbCount)
SetDbLocalFuncType(ActivityCountDbBox.RecordActionTimes)
SetDbLocalFuncType(ActivityCountDbBox.GetGuideInfo)

return ActivityCountDbBox

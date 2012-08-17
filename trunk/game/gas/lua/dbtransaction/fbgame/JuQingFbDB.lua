--剧情本
cfg_load "fb_game/JuQingTransmit_Common"

local JuQingTransmit_Common = JuQingTransmit_Common



local StmtContainer = class()
local event_type_tbl = event_type_tbl
local JuQingFbDB = CreateDbBox(...)

function JuQingFbDB.FbCheck(charId, ActionName, Member, IsAutoTeam)
	local Level = JuQingTransmit_Common(ActionName, "Level")
	
	local ExChanger = RequireDbBox("Exchanger")
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local ActivityCountDbBox = RequireDbBox("ActivityCountDB")
	
	if not IsAutoTeam then
		local GasTeamDB =  RequireDbBox("GasTeamDB")
		local teamId = GasTeamDB.GetTeamID(charId)
		if teamId == 0 then
			return false, {191035}
		end
	end
	
	for _, id in pairs(Member) do
		local sName = ExChanger.getPlayerNameById(id)
		
		--检测等级
		local MemberLevel = CharacterMediatorDB.GetPlayerLevel(id)
		if MemberLevel < Level then
			if charId == id then
				return false, {191010, Level}
			else
				return false, {191009, Level}
			end
		end
		
		--检测是否已经参加过了
		local mercParam = {}
		mercParam["PlayerId"] = id
		mercParam["ActivityName"] = "剧情本限次"
		
		local Name = ExChanger.getPlayerNameById(id)
		local IsAllow, FinishTimes = ActivityCountDbBox.CheckSinglePlayerActivityCount(mercParam)
		
		--30级以前一天2次
		--30-40级 一天4次
		--40-50级 一天6次
		--50-60级 一天8次
		--60以上  一天10次
		--print(FinishTimes)
		if IsAllow then
			if MemberLevel < 30 then
				if FinishTimes >= 2 then
					IsAllow = false
				end
			elseif MemberLevel >= 30 and MemberLevel < 40 then
				if FinishTimes >= 4 then
					IsAllow = false
				end
			elseif MemberLevel >= 40 and MemberLevel < 50 then
				if FinishTimes >= 6 then
					IsAllow = false
				end
			elseif MemberLevel >= 50 and MemberLevel < 60 then
				if FinishTimes >= 8 then
					IsAllow = false
				end
			else
				if FinishTimes >= 10 then
					IsAllow = false
				end
			end
		end
		
		if not IsAllow then
			return false, {3511, Name}
		end
	end
	
	return true
end



local StmtDef =
{
	"_GetKillBossTime",
	"select unix_timestamp(ac_dtTime) from tbl_activity_count where cs_uId = ? and ac_sActivityName = ?"
}
DefineSql( StmtDef , StmtContainer )

local StmtDef =
{
	"_InsertWeekAwardState",
	"insert into tbl_activity_count (cs_uId,ac_sActivityName,ac_dtTime) values (?,?,now())"
}
DefineSql( StmtDef , StmtContainer )

local StmtDef =
{
	"_UpdateWeekAwardState",
	[[
		update
			tbl_activity_count
		set
			cs_uId = ?,
			ac_sActivityName = ?,
			ac_dtTime = now()
		where
			cs_uId = ? and ac_sActivityName = ?
	]]
}
DefineSql( StmtDef , StmtContainer )

--加一个判断,是当天完成的返回true
local function DateCompare(oldDate)
	local Time = 5--时间刻度
	
	local nowDate = os.date("*t",os.time())
	local dbDate = os.date("*t",oldDate)
	if (nowDate.year == dbDate.year)
		and (nowDate.month == dbDate.month) then
			
			local day = nowDate.day - dbDate.day
			if day == 0 then--如果是同一天的(相同日期)
				if (nowDate.hour < Time and dbDate.hour < Time) 
					or (nowDate.hour >= Time and dbDate.hour >= Time) then
					return true
				end
			elseif day == 1 then--如果昨天参加了(不同日期)
				if nowDate.hour < Time and dbDate.hour >= Time then
					return true
				end
			end
			
	end
	return false
end

local function CheckIsFinish(charID, NpcName)
	local result = StmtContainer._GetKillBossTime:ExecStat(charID, NpcName)
	if result:GetRowNum() > 0 then
		--加一个判断,如果是当天完成的,就不让再给积分了
		if DateCompare(result:GetData(0,0)) then
			return false
		end
		
		StmtContainer._UpdateWeekAwardState:ExecStat(charID, NpcName, charID, NpcName)
	else
		StmtContainer._InsertWeekAwardState:ExecStat(charID, NpcName)
	end
	
	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
		return true
	else
		return false
	end
end

function JuQingFbDB.AddPointForPlayerTbl(data)
	local SceneName = data["SceneName"]
	local PlayerInfo = data["PlayerInfo"]
	local NpcName = data["NpcName"]
	local ExChanger = RequireDbBox("Exchanger")
	local AreaFbPointDB = RequireDbBox("AreaFbPointDB")
	
	local PointTbl = {}
	for uCharId,Info in pairs(PlayerInfo) do
		
		if CheckIsFinish(uCharId, "剧情本:"..NpcName) then
			local res = AreaFbPointDB.AddAreaFbPointByType(uCharId,Info["Point"],5,SceneName,nil,event_type_tbl["竞技场副本加积分"])
			if not res then
				CancelTran()
				return false
			end
			
			PointTbl[uCharId] = {ExChanger.getPlayerNameById(uCharId),Info["Point"]}
		else
			PointTbl[uCharId] = {ExChanger.getPlayerNameById(uCharId),0}
		end
		
	end
	return true, PointTbl
end



--得到今天已经完成过的副本
local StmtDef =
{
	"_GetFinishAction",
	[[
		select
			ac_sActivityName,unix_timestamp(ac_dtTime)
		from
			tbl_activity_count
		where
			ac_sActivityName like "%剧情本:%" and cs_uId = ?
	]]
}
DefineSql( StmtDef , StmtContainer )

function JuQingFbDB.GiveFinishAction(data)
	local PlayerId = data["PlayerId"]
	local AreaFbPointDB = RequireDbBox("AreaFbPointDB")
	local ActionTbl = {}
	
	local res = StmtContainer._GetFinishAction:ExecStat(PlayerId)
	if res:GetRowNum() > 0 then
		for i=1, res:GetRowNum() do
			local BossName = res:GetData(i-1, 0)
			
			if DateCompare(res:GetData(i-1,1)) then
				BossName = string.gsub(BossName,"剧情本:","")
				
				local keys = JuQingTransmit_Common:GetKeys()
				for id=1, #(keys) do
					local JuQingCommon = JuQingTransmit_Common(keys[id])
					if BossName == JuQingCommon("BossName") then
						table.insert(ActionTbl, JuQingCommon("ActionName"))
						break
					end
				end
				
			end
		end
	end
	
	return true, ActionTbl
end

SetDbLocalFuncType(JuQingFbDB.GiveFinishAction)
SetDbLocalFuncType(JuQingFbDB.AddPointForPlayerTbl)
return JuQingFbDB

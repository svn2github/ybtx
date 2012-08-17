local os = os
local g_ItemInfoMgr = CItemInfoMgr:new()

local event_type_tbl = event_type_tbl

local StmtContainer = class()

----------------------------------------------

local StmtDef=
{
	"_UpdatePlayerPoint",
	[[
		update
			tbl_char_jifensaipoint
		set
			cs_uWinNum = cs_uWinNum + ?,
			cs_uLoseNum = cs_uLoseNum + ?,
			cs_uRunNum = cs_uRunNum + ?,
			cs_uPoint = cs_uPoint + ?,
			cj_uInFbTimes = ?,
			cj_dDate = from_unixtime(?),
			cj_uKillNum = cj_uKillNum + ?,
			cj_uDeadNum = cj_uDeadNum + ?
		where
			(cs_uPoint + ?) > 20 and cs_uId = ? and cs_uType = ?
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_GetPlayerInFbTimes",
	"select cs_uType,cj_uInFbTimes,unix_timestamp(cj_dDate) from tbl_char_jifensaipoint where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_GetPlayerMatchInfo",
	"select cs_uWinNum,cs_uLoseNum,cs_uRunNum,cs_uPoint from tbl_char_jifensaipoint where cs_uId = ? and cs_uType = ?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_GetPlayerPanelInfo",
	"select cs_uType,cs_uWinNum,cs_uLoseNum,cs_uRunNum,cj_uKillNum,cj_uDeadNum,cs_uPoint from tbl_char_jifensaipoint where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)
---------------------------------------------------



--------------------------------------------------
local StmtDef=
{
	"_InserPointAccum",
	[[
	insert into	tbl_char_jifensaiaccum
		(cs_uId,cs_uType,cs_uUpWeekWinNum,cs_uUpWeekLoseNum,cs_uUpWeekRunNum,cs_uUpWeekPoint,cs_dUpDate,cs_uCurrWeekWinNum,cs_uCurrWeekLoseNum,cs_uCurrWeekRunNum,cs_uCurrWeekPoint,cs_dCurrDate)
	values
		(?,?,?,?,?,?,from_unixtime(?),?,?,?,?,from_unixtime(?))
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_UpdateCurrWeekPointAccum",
	[[
	update
		tbl_char_jifensaiaccum
	set
		cs_uCurrWeekWinNum = cs_uCurrWeekWinNum + ?,
		cs_uCurrWeekLoseNum = cs_uCurrWeekLoseNum + ?,
		cs_uCurrWeekRunNum = cs_uCurrWeekRunNum + ?,
		cs_uCurrWeekPoint = cs_uCurrWeekPoint + ?,
		cs_dCurrDate = from_unixtime(?)
	where
		cs_uId = ? and cs_uType = ?
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_UpdateAllPointAccum",
	[[
	update
		tbl_char_jifensaiaccum
	set
		cs_uUpWeekWinNum = cs_uUpWeekWinNum + ?,
		cs_uUpWeekLoseNum = cs_uUpWeekLoseNum + ?,
		cs_uUpWeekRunNum = cs_uUpWeekRunNum + ?,
		cs_uUpWeekPoint = cs_uUpWeekPoint + ?,
		cs_dUpDate = from_unixtime(?),
		cs_uCurrWeekWinNum = cs_uCurrWeekWinNum + ?,
		cs_uCurrWeekLoseNum = cs_uCurrWeekLoseNum + ?,
		cs_uCurrWeekRunNum = cs_uCurrWeekRunNum + ?,
		cs_uCurrWeekPoint = cs_uCurrWeekPoint + ?,
		cs_dCurrDate = from_unixtime(?)
	where
		cs_uId = ? and cs_uType = ?
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_GetPointAccumData",
	"select cs_uCurrWeekWinNum,cs_uCurrWeekLoseNum,cs_uCurrWeekRunNum,cs_uCurrWeekPoint,unix_timestamp(cs_dCurrDate) from tbl_char_jifensaiaccum where	cs_uId = ? and cs_uType = ?"
}
DefineSql(StmtDef,StmtContainer)
----------------------------------------------------------------------
local StmtDef =
{
	"_GetPlayerLevel",
	"select cb_uLevel from tbl_char_basic where cs_uId = ? "
}
DefineSql( StmtDef , StmtContainer )

local JiFenSaiFb = CreateDbBox(...)

local function GetWinExp(charId)
	local level_result = StmtContainer._GetPlayerLevel:ExecStat(charId)
	local Level = level_result:GetData(0,0)
	local Exp = 0.5 * 5 * (Level * 6 + 50) / 10 * 60 * 8 * 0.8
	return math.floor(Exp)
end

local function GetLoseExp(charId)
	local level_result = StmtContainer._GetPlayerLevel:ExecStat(charId)
	local Level = level_result:GetData(0,0)
	local Exp = 0.5 * 5 * (Level * 6 + 50) / 10 * 60 * 3 * 0.8
	return math.floor(Exp)
end

-- 插入角色积分赛的积分信息
local StmtDef=
{
	"InsertJiFenSaiPoint",
	"insert into tbl_char_jifensaipoint (cs_uId,cs_uType) values (?,?)"
}
DefineSql(StmtDef,StmtContainer)

function JiFenSaiFb.insertJiFenSaiPoint(uCharId,Type)
	if uCharId and Type then
		StmtContainer.InsertJiFenSaiPoint:ExecSql("",uCharId,Type)
		return g_DbChannelMgr:LastAffectedRowNum() > 0
	end
end

function JiFenSaiFb.updatePlayerPoint(uCharId,Type,WinNum,LoseNum,RunNum,Point,MatchTimes,Time,KillNum,DeadNum)
	if uCharId then
		StmtContainer._UpdatePlayerPoint:ExecSql("",WinNum,LoseNum,RunNum,Point,MatchTimes,Time,KillNum,DeadNum,Point,uCharId,Type)
		return g_DbChannelMgr:LastAffectedRowNum() > 0
	end
end

function JiFenSaiFb.InsertPointAccum(uCharId,Type,WinNum,LoseNum,RunNum,Point,Date)
	if Point and uCharId and Type and Date then
		StmtContainer._InserPointAccum:ExecSql("",uCharId,Type,0,0,0,0,0,WinNum,LoseNum,RunNum,Point,Date)
		return g_DbChannelMgr:LastAffectedRowNum() > 0
	end
end

local StmtDef=
{
	"_GetPlayerTongID",
	[[
		select
			t_uId
		from
			tbl_member_tong
		where
			cs_uId = ?
	]]
}
DefineSql(StmtDef, StmtContainer)
local function GetPlayerTongID(charID)
	local res = StmtContainer._GetPlayerTongID:ExecSql("n",charID)
	local num = res:GetRowNum()
	if num > 0 then
		local TongID = res:GetData(0,0)
		res:Release()
		return TongID
	end
end

local function CheckFbTimes(PlayerId, ActionName)	
	local ActivityCountDB = RequireDbBox("ActivityCountDB")
	local isCan, count = ActivityCountDB.CheckFbActionCount(PlayerId, ActionName)
	if isCan then
		return true
	else
		return false, count
	end
end

function JiFenSaiFb.CheckTeamMemberNum(data)
	local charId = data["charID"]
	local JoinActionDB = RequireDbBox("JoinActionDB")
	if not JoinActionDB.CheckWarnValue(charId) then
		return false
	end
	
	local ActionName = data["ActionName"]
	local TeamNum = data["TeamNum"] or 3
	
	local GasTeamDB = RequireDbBox("GasTeamDB")
	local ExChanger = RequireDbBox("Exchanger")
	
	local teamId = GasTeamDB.GetTeamID(charId)
	if teamId == 0 then
		return false,3408				--"个人不能参加团队赛"
	else
		local captain = GasTeamDB.GetCaptain(teamId)
		if captain ~= charId then
			return false,3410			--"你不是团队的队长,不能报名参加"
		else
			local CaptainTongID = GetPlayerTongID(charId)
			if not CaptainTongID then
				return false,	3425			--你没有加入一个帮会,不能参加比赛
			end
			local charTbl = GasTeamDB.GetTeamOnlineMembersId(teamId)
			local CoutPlayerNum = #(charTbl)
			if CoutPlayerNum ~= TeamNum then --有可能连自己都不在线了,直接返回
				return false, 3411, {TeamNum}		--"团队参赛,要求2V2,3V3或者是5V5"
			else
				local MinLevel = 150
				local MaxLevel = 1
				for _, id in pairs(charTbl) do
					if id ~= charId and not JoinActionDB.CheckWarnValue(id, charId) then
						return false
					end
					
					local tongID = GetPlayerTongID(id)
					if not tongID then
						local name = ExChanger.getPlayerNameById(id)
						return false,	3426, {name}		--队伍中的某人,没有加入帮会
					end
					
--					if CaptainTongID ~= tongID then
--						return false,	3427		--队伍不是同一个帮会的成员,不能报名
--					end
					
					local succ, count = CheckFbTimes(id, ActionName)
					if not succ then--队伍中的某人,已经参加了N次了
						local name = ExChanger.getPlayerNameById(id)
						return false, 3428, {name, count, ActionName}
					end
					
					local level_result = StmtContainer._GetPlayerLevel:ExecSql('n',id)
					local Level = level_result:GetData(0,0)
					
					if MinLevel > Level then
						MinLevel = Level
					end
					if MaxLevel < Level then
						MaxLevel = Level
					end
				end
				
				if (MaxLevel - MinLevel) > 10 then
					return false, 3429
				end
				
				return true
			end
		end
	end
end

function JiFenSaiFb.GetPlayerMatchTimes(PlayerId)
	local MatchTimes = 0
	local EndTime = 0
	
	local res = StmtContainer._GetPlayerInFbTimes:ExecSql("nnn",PlayerId)
	if res then
		local RowNum = res:GetRowNum()
		if RowNum > 0 then
			for i=0,RowNum-1 do
				local Date = res:GetData(i,2)
				if Date > EndTime then
					EndTime = Date
					MatchTimes = res:GetData(i,1)
				end
			end
			res:Release()
		end
	end
	return MatchTimes,EndTime
end

local function IsSameDay(EndTime,NowTime)
	if NowTime == nil or EndTime == nil then
		return false
	end
	local date1 = os.date("*t", NowTime)
	local date2 = os.date("*t", EndTime)
	if date1.year == date2.year and date1.month == date2.month and date1.day == date2.day then
		return true
	else
		return false
	end
end

local function UpdateAllPoint(PlayerId,State,TeamType,MatchPoint,GloryPoint,Time,IsWin,KillNum,DeadNum)
	local WinNum = 0
	local LoseNum = 0
	local RunNum = 0
	if State == 1 then
		WinNum = 1
	elseif State == 2 then
		LoseNum = 1
	else
		GloryPoint = 0--逃跑的没有荣誉积分
		RunNum = 1
	end
	
	local WinCoef = 1
	if not IsWin then
		WinCoef = -1
	end
	MatchPoint = MatchPoint * WinCoef
	
	if RunNum == 0 and GloryPoint > 0 then	--没有逃跑的,给联赛荣誉值
		local AreaFbPointDB = RequireDbBox("AreaFbPointDB")
		AreaFbPointDB.AddAreaFbPointByType(PlayerId,GloryPoint,5,"竞技场副本",nil,event_type_tbl["竞技场副本加积分"])
	end
	
	local MatchTimes = 0
	JiFenSaiFb.updateJiFenSaiWeekPoint(PlayerId,TeamType,WinNum,LoseNum,RunNum,GloryPoint,Time)
	local Istrue = JiFenSaiFb.updatePlayerPoint(PlayerId,TeamType,WinNum,LoseNum,RunNum,MatchPoint,MatchTimes,Time,KillNum,DeadNum)
	return Istrue,MatchTimes,GloryPoint
end

--荣誉积分
local function GetPlayerGloryPoint(charId, TeamType,IsWin)--得到荣誉积分
	local Coef = {18,27,37,47,57,67,78,88,98,108,118,129,139,149,159,169}
	
	local level_result = StmtContainer._GetPlayerLevel:ExecStat(charId)
	local PlayerLevel = level_result:GetData(0,0)
	level_result:Release()
	
	local Index = 1
	if PlayerLevel < 150 then
		Index = math.floor(PlayerLevel/10)+1
	else
		Index = 16
	end
	
	if IsWin then
		local PlayerNum = 4
		if TeamType == 2 then
			PlayerNum = 6
		elseif TeamType == 3 then
			PlayerNum = 10
		end
		return PlayerNum * Coef[Index]
	else
		return Coef[Index]
	end
end

local function AddExpAndTongPoint(PlayerId, addExp)
	local LevelInfo = nil
	if addExp and addExp ~= 0 then
		local param = {}
		param["char_id"] = PlayerId
		param["addExp"] = addExp
		param["addExpType"] = event_type_tbl["副本奖励"]
		local RoleQuestDB = RequireDbBox("RoleQuestDB")
		local CurLevel,LevelExp = RoleQuestDB.AddExp(param)
		if CurLevel then
			LevelInfo = {}
			LevelInfo["CurLevel"] = CurLevel
			LevelInfo["LevelExp"] = LevelExp
			LevelInfo["addExp"] = addExp
		end
	end
	
	return LevelInfo
end

function JiFenSaiFb.JfsAddExp(data)
	local charId = data["charId"]
	local LevelInfo = AddExpAndTongPoint(charId, GetLoseExp(charId))
	return LevelInfo
end

--加帮会荣誉值
local function AddTongHouor(charId, uHonor)
	local TongID = GetPlayerTongID(charId)
	if TongID then
		local parameters = {}
		parameters["uTongID"] = TongID
		parameters["uHonor"] = uHonor
		parameters["nEventType"] = 125
		local GasTongBasicDB = RequireDbBox("GasTongBasicDB")
		GasTongBasicDB.UpdateHonor(parameters)
		return true
	end
end

local function GetProffer(WinPoint, LosePoint, IsWin)
	local Cofe = 1
	if not IsWin then
		Cofe = -1
	end
	if WinPoint == 0 or LosePoint == 0 then
		return 0
	else
		return math.floor((WinPoint + LosePoint)/6*10 + (Cofe * (LosePoint*(LosePoint/WinPoint))))
	end
end

--加团贡值
local function AddProffer(charId, count)
	if count ~= 0 then
		local AreaFbPointDB = RequireDbBox("AreaFbPointDB")
		local IsSucc, _ = AreaFbPointDB.AddAreaFbPointByType(charId,count,10,"竞技场副本",nil,event_type_tbl["竞技场副本加积分"])
		if IsSucc then
			return count
		end
	end
	return 0
end

function JiFenSaiFb.UpdateDataInfo(data)
	local DeadRunAwayTbl = data["DeadRunAwayTbl"]
	local PlayerKillNumTbl = data["PlayerKillNumTbl"]
	local PlayerDeadNumTbl = data["PlayerDeadNumTbl"]
	
	if DeadRunAwayTbl and IsTable(DeadRunAwayTbl) then
		local TeamType = data["TeamType"]
		local Time = data["time"]
		
		local MatchPoint = 0
		
		local WinPoint = 0
		local LosePoint = 0
		if not data["IsAllExit"] then
			for TeamID,tbl in pairs(DeadRunAwayTbl) do
				if tbl and IsTable(tbl) then
					if TeamID == data["DeadTeamID"] then
						for id,IsRuning in pairs(tbl) do
							local level_result = StmtContainer._GetPlayerLevel:ExecStat(id)
							local Level = level_result:GetData(0,0)
							level_result:Release()
							LosePoint = LosePoint + Level
						end
					else
						for id,IsRuning in pairs(tbl) do
							local level_result = StmtContainer._GetPlayerLevel:ExecStat(id)
							local Level = level_result:GetData(0,0)
							level_result:Release()
							WinPoint = WinPoint + Level
						end
					end
				end
			end
		end
		
		local PlayerInfo = {}
		for TeamID,tbl in pairs(DeadRunAwayTbl) do
			
			if tbl and IsTable(tbl) then
				
				if TeamID == data["DeadTeamID"] or data["IsAllExit"] then
					
					for id,IsRuning in pairs(tbl) do
						local iMatchPoint = MatchPoint
						local State = 2
						if IsRuning == 3 then
							State = 0
							iMatchPoint = 0
						end
						
						local Proffer = 0
						if not data["IsAllExit"] and State ~= 0 then
							Proffer = AddProffer(id, GetProffer(WinPoint, LosePoint, false))
						end
						
						--local PlayerGloryPoint = GetPlayerGloryPoint(id, TeamType, false)
						local Istrue,MatchTimes,GloryPoint = UpdateAllPoint(id,State,TeamType,iMatchPoint,0,Time,false,PlayerKillNumTbl[id],PlayerDeadNumTbl[id])
--						local CountPointTbl = JiFenSaiFb.getPlayerPoint(id)--得到3个类型的积分
						
						local MatchInfo = nil
						if IsRuning == 1 or IsRuning == 2 then
							MatchInfo = JiFenSaiFb.GetPlayerMatchInfo({["PlayerId"] = id,["TeamType"] = TeamType})
							if MatchInfo then
								local iKillNum = PlayerKillNumTbl[id]
								if not iKillNum then
									iKillNum = 0
								end
								MatchInfo = iKillNum..","..Proffer..","..MatchInfo
							end
						end
						
						local retParam = {}
						retParam["PlayerId"] = id
--						retParam["MatchPoint"] = iMatchPoint*-1
--						retParam["CountPoint"] = CountPointTbl[TeamType]
						retParam["PlayerState"] = IsRuning
						retParam["MatchTimes"] = 50-MatchTimes
						retParam["MatchInfo"] = MatchInfo
						retParam["GloryPoint"] = GloryPoint
						
						table.insert(PlayerInfo,retParam)
					end
				else
					
					for id,IsRuning in pairs(tbl) do
						local iMatchPoint = MatchPoint
						local State = 1
						if IsRuning == 3 then
							State = 0
							iMatchPoint = 0
						end
						
						local Proffer = 0
						if State ~= 0 then
							Proffer = AddProffer(id, GetProffer(WinPoint, LosePoint, true))
						end
						
						--local PlayerGloryPoint = GetPlayerGloryPoint(id, TeamType, true)
						local Istrue,MatchTimes,GloryPoint = UpdateAllPoint(id,State,TeamType,iMatchPoint,0,Time,true,PlayerKillNumTbl[id],PlayerDeadNumTbl[id])
--						local CountPointTbl = JiFenSaiFb.getPlayerPoint(id)--得到3个类型的积分
						
						local MatchInfo = nil
						if IsRuning == 1 or IsRuning == 2 then
							MatchInfo = JiFenSaiFb.GetPlayerMatchInfo({["PlayerId"] = id,["TeamType"] = TeamType})
							if MatchInfo then
								local iKillNum = PlayerKillNumTbl[id]
								if not iKillNum then
									iKillNum = 0
								end
								MatchInfo = iKillNum..","..Proffer..","..MatchInfo
							end
						end
						
--						local LevelInfo = nil
--						if IsRuning == 1 then
--							LevelInfo = AddExpAndTongPoint(id, GetWinExp(id))
--						end
						
						local retParam = {}
						retParam["PlayerId"] = id
--						retParam["MatchPoint"] = iMatchPoint
--						retParam["CountPoint"] = CountPointTbl[TeamType]
						retParam["PlayerState"] = IsRuning
						retParam["MatchTimes"] = 50-MatchTimes
						retParam["MatchInfo"] = MatchInfo
						retParam["GloryPoint"] = GloryPoint
--						retParam["LevelInfo"] = LevelInfo
						
						table.insert(PlayerInfo,retParam)
					end
				end
				
			end
			
		end
		
		return PlayerInfo
	else
		return false
	end
end

--判断是不是想要的那一周
--传入0,判断是不是当前周
--传入1,判断是不是上一周
local function IsSameWeek(OldDate, CurrDate, WeekNum)
	if OldDate == nil or CurrDate == nil then
		return false
	end
	
	local DayTime = 24*60*60	--一天的秒数
	local WeekTime = 7*DayTime	--周的秒数
	
	local NowDay = 1
	local date = os.date("*t",OldDate)
	local DayCoef = date.wday
	if DayCoef > NowDay then				--要用到那一天(即星期几,这一天就用1代表)
		DayCoef = DayCoef - NowDay
	else
		DayCoef = DayCoef + (7-NowDay)
	end
	
	local InitWeekTime = (DayCoef-1) * DayTime --这些天的秒数(为了让时间全部回到初始状态,例如:周日[周日为每一周的第一天])
	
	local WeekCoef = math.floor((CurrDate-(OldDate-InitWeekTime)) / WeekTime)
	
	if WeekCoef == WeekNum then
		--print("...........in same")
		return true
	else
		--print("...........in not same")
		return false
	end
end

function JiFenSaiFb.updateJiFenSaiWeekPoint(uCharId,Type,WinNum,LoseNum,RunNum,Point,Time)
	local res = StmtContainer._GetPointAccumData:ExecSql("nnnnn",uCharId,Type)
	if not res or res:GetRowNum() <= 0 then
		res:Release()
		JiFenSaiFb.InsertPointAccum(uCharId,Type,WinNum,LoseNum,RunNum,Point,Time)
	else
		local CurrWeekWinNum = res:GetData(0,0)
		local CurrWeekLoseNum = res:GetData(0,1)
		local CurrWeekRunNum = res:GetData(0,2)
		local CurrWeekPoint = res:GetData(0,3)
		local Data = res:GetData(0,4)
		if IsSameWeek(Data,Time,0) then
			StmtContainer._UpdateCurrWeekPointAccum:ExecSql('',WinNum,LoseNum,RunNum,Point,Time,uCharId,Type)
		else
			StmtContainer._UpdateAllPointAccum:ExecSql('',CurrWeekWinNum,CurrWeekLoseNum,CurrWeekRunNum,CurrWeekPoint,Data,WinNum,LoseNum,RunNum,Point,Time,uCharId,Type)
		end
	end
end

function JiFenSaiFb.GetPlayerMatchInfo(data)
	local PlayerId = data["PlayerId"]
	local TeamType = data["TeamType"]
	
	local res1 = StmtContainer._GetPlayerMatchInfo:ExecSql('nnnn',PlayerId,TeamType)
	local res2 = StmtContainer._GetPointAccumData:ExecSql('nnnnn',PlayerId,TeamType)
	
	if res1 and res2 then
		if res1:GetRowNum() > 0 and res2:GetRowNum() > 0 then
			
			local WinNum = res1:GetData(0,0)
			local LoseNum = res1:GetData(0,1)
			local RunNum = res1:GetData(0,2)
			local Point = res1:GetData(0,3)
			
			local CurrWeekWinNum = res2:GetData(0,0)
			local CurrWeekLoseNum = res2:GetData(0,1)
			local CurrWeekRunNum = res2:GetData(0,2)
			local CurrWeekPoint = res2:GetData(0,3)
			
			local wMatchTimes = CurrWeekWinNum + CurrWeekLoseNum + CurrWeekRunNum--比赛次数
			local cMatchTimes = WinNum + LoseNum + RunNum--总的比赛次数
			
			res1:Release()
			res2:Release()
			return wMatchTimes..","..cMatchTimes..","..CurrWeekWinNum..","..WinNum
		end
	end
end

function JiFenSaiFb.GetLevelAndPoint(CharIDTbl, RoomType)
	local MaxLevel = 0
	local CountPoint = 0
	local CaptainTongID = 0
	for _,charId in pairs(CharIDTbl) do
		if CaptainTongID == 0 then
			CaptainTongID = GetPlayerTongID(charId) or 0
		end
		local level_result = StmtContainer._GetPlayerLevel:ExecSql('n',charId)
		local Level = level_result:GetData(0,0)
		level_result:Release()
		if Level > MaxLevel then
			MaxLevel = Level
		end
--		local CountPointTbl = JiFenSaiFb.getPlayerPoint(charId)--得到3个类型的积分
--		if CountPointTbl[RoomType] > CountPoint then
--			CountPoint = CountPointTbl[RoomType]
--		end
		CountPoint = CountPoint + Level
	end
	
	return MaxLevel, CountPoint, CaptainTongID
end

function JiFenSaiFb.GetPlayerPanelInfo(data)
	local res = StmtContainer._GetPlayerPanelInfo:ExecSql('nnnnnnn',data["CharId"])
	local infoTbl = {}
	for i=0, 2 do
		if not res or res:GetRowNum()<=0 then
			local type = i+1
			infoTbl[type] = {}
			infoTbl[type].WinNum 	= 0
			infoTbl[type].LoseNum = 0
			infoTbl[type].RunNum 	= 0
			infoTbl[type].WinPro 	= 0.0
			infoTbl[type].KillNum = 0
			infoTbl[type].DeadNum = 0
			infoTbl[type].Point = 20000
		else
			local type = res:GetData(i, 0)
			infoTbl[type] = {}
			infoTbl[type].WinNum 	= res:GetData(i, 1)
			infoTbl[type].LoseNum = res:GetData(i, 2)
			infoTbl[type].RunNum 	= res:GetData(i, 3)
			local inFbTimes = infoTbl[type].WinNum + infoTbl[type].LoseNum + infoTbl[type].RunNum
			if inFbTimes == 0 then
				infoTbl[type].WinPro = 0.0
			else
				infoTbl[type].WinPro = infoTbl[type].WinNum / inFbTimes
			end
			infoTbl[type].KillNum = res:GetData(i, 4)
			infoTbl[type].DeadNum = res:GetData(i, 5)
			infoTbl[type].Point = res:GetData(i, 6)
		end
	end
	res:Release()
	return infoTbl
end


local StmtDef = {
	"_AddJfsKillNumInfo",
	"insert ignore into tbl_tong_jfs (cs_uId, tj_uNum) values (?, 1) "
}
DefineSql ( StmtDef, StmtContainer )

local StmtDef = {
	"_GetJfsKillNumInfo",
	"select tj_uNum from tbl_tong_jfs where cs_uId = ?"
}
DefineSql ( StmtDef, StmtContainer )

local StmtDef = {
	"_UpdateJfsKillNumInfo",
	"update tbl_tong_jfs set tj_uNum = tj_uNum + 1 where cs_uId = ?"
}
DefineSql ( StmtDef, StmtContainer )

function JiFenSaiFb.UpdateKillNumSort(data)
	local charId = data[1]
	
	local res = StmtContainer._GetJfsKillNumInfo:ExecStat(charId)
	if res and res:GetRowNum()>0 then
		StmtContainer._UpdateJfsKillNumInfo:ExecStat(charId)
	else
		StmtContainer._AddJfsKillNumInfo:ExecStat(charId)
	end
end


SetDbLocalFuncType(JiFenSaiFb.UpdateKillNumSort)
return JiFenSaiFb
